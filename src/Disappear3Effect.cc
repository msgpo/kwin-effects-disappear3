/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Disappear3Effect.h"

// KConfigSkeleton
#include "disappear3config.h"

namespace {
const int Disappear3WindowRole = 0x22A98500;
}

Disappear3Effect::Disappear3Effect()
{
    initConfig<Disappear3Config>();
    reconfigure(ReconfigureAll);

    connect(KWin::effects, &KWin::EffectsHandler::windowAdded,
        this, &Disappear3Effect::markWindow);
    connect(KWin::effects, &KWin::EffectsHandler::windowClosed,
        this, &Disappear3Effect::start);
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted,
        this, &Disappear3Effect::stop);
}

Disappear3Effect::~Disappear3Effect()
{
}

void Disappear3Effect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags);

    Disappear3Config::self()->read();
    m_blacklist = Disappear3Config::blacklist().toSet();
    m_duration = animationTime(Disappear3Config::duration() > 0
            ? Disappear3Config::duration()
            : 120);
    m_opacity = Disappear3Config::opacity();
    m_distance = Disappear3Config::distance();
}

void Disappear3Effect::prePaintScreen(KWin::ScreenPrePaintData& data, int time)
{
    auto it = m_animations.begin();
    while (it != m_animations.end()) {
        Timeline& t = it.value();
        t.update(time);
        if (t.done()) {
            KWin::EffectWindow* w = it.key();
            w->unrefWindow();
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }

    if (!m_animations.isEmpty()) {
        data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;
    }

    KWin::effects->prePaintScreen(data, time);
}

void Disappear3Effect::prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, int time)
{
    if (m_animations.contains(w)) {
        w->enablePainting(KWin::EffectWindow::PAINT_DISABLED_BY_DELETE);
        data.setTransformed();
    }

    KWin::effects->prePaintWindow(w, data, time);
}

void Disappear3Effect::paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data)
{
    const auto it = m_animations.constFind(w);
    if (it != m_animations.cend()) {
        const qreal t = (*it).value();

        data.setZTranslation(interpolate(0, m_distance, t));
        data.multiplyOpacity(interpolate(1, m_opacity, t));
    }

    KWin::effects->paintWindow(w, mask, region, data);
}

void Disappear3Effect::postPaintScreen()
{
    if (!m_animations.isEmpty()) {
        KWin::effects->addRepaintFull();
    }

    KWin::effects->postPaintScreen();
}

bool Disappear3Effect::isActive() const
{
    return !m_animations.isEmpty();
}

bool Disappear3Effect::supported()
{
    return KWin::effects->isOpenGLCompositing()
        && KWin::effects->animationsSupported();
}

bool Disappear3Effect::shouldAnimate(const KWin::EffectWindow* w) const
{
    if (KWin::effects->activeFullScreenEffect()) {
        return false;
    }

    const auto* closeGrab = w->data(KWin::WindowClosedGrabRole).value<void*>();
    if (closeGrab != nullptr && closeGrab != this) {
        return false;
    }

    if (m_blacklist.contains(w->windowClass())) {
        return false;
    }

    if (w->data(Disappear3WindowRole).toBool()) {
        return true;
    }

    if (!w->isManaged()) {
        return false;
    }

    return w->isNormalWindow()
        || w->isDialog();
}

void Disappear3Effect::start(KWin::EffectWindow* w)
{
    if (!shouldAnimate(w)) {
        return;
    }

    // Tell other effects(like fade, for example) to ignore this window.
    w->setData(KWin::WindowClosedGrabRole, QVariant::fromValue(static_cast<void*>(this)));

    w->refWindow();

    Timeline& t = m_animations[w];
    t.setDuration(m_duration);
    t.setEasingCurve(QEasingCurve::OutCurve);
}

void Disappear3Effect::stop(KWin::EffectWindow* w)
{
    m_animations.remove(w);
}

void Disappear3Effect::markWindow(KWin::EffectWindow* w)
{
    if (!shouldAnimate(w)) {
        return;
    }
    w->setData(Disappear3WindowRole, true);
}
