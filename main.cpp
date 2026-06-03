#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Capas concéntricas de glow (más = gradiente más suave)
static constexpr int GLOW_STEPS = 6;

// ─────────────────────────────────────────────────────────────────────────────
//  DirectionBar
//  CCNode que muestra una barra sólida + glow opcional.
//    · Amarillo  →  jugador subiendo  (velocidad Y positiva)
//    · Azul      →  jugador bajando   (velocidad Y negativa / en suelo)
// ─────────────────────────────────────────────────────────────────────────────
class DirectionBar : public CCNode {
private:
    CCLayerColor* m_bar = nullptr;
    CCLayerColor* m_glowLayers[GLOW_STEPS] = {};

    bool m_goingUp     = false;
    bool m_initialized = false;

    // Valores cacheados para detectar cambios en settings sin hacer rebuild
    // cada fotograma
    float m_cBW       = -1.f;
    float m_cBH       = -1.f;
    float m_cGlowSz   = -1.f;
    int   m_cGlowAlpha = -1;
    bool  m_cUseGlow   = false;

public:
    static DirectionBar* create() {
        auto* node = new DirectionBar();
        if (node && node->init()) {
            node->autorelease();
            return node;
        }
        CC_SAFE_DELETE(node);
        return nullptr;
    }

    bool init() override {
        if (!CCNode::init()) return false;
        this->setID("direction-bar");
        return true;
    }

    // Sincroniza la posición con los settings (llamado cada frame, es barato)
    void syncPosition() {
        auto* mod = Mod::get();
        auto x = static_cast<float>(mod->getSettingValue<double>("pos-x"));
        auto y = static_cast<float>(mod->getSettingValue<double>("pos-y"));
        this->setPosition({x, y});
    }

    // Punto de entrada principal: recibe la dirección actual del jugador
    void setDirection(bool goingUp) {
        auto* mod      = Mod::get();
        auto bw        = static_cast<float>(mod->getSettingValue<double>("bar-width"));
        auto bh        = static_cast<float>(mod->getSettingValue<double>("bar-height"));
        auto useGlow   = mod->getSettingValue<bool>("use-glow");
        auto glowSz    = static_cast<float>(mod->getSettingValue<double>("glow-size"));
        auto glowAlpha = static_cast<int>(mod->getSettingValue<int64_t>("glow-opacity"));

        // Si cambiaron los settings visuales, reconstruimos todo desde cero
        bool settingsChanged = !m_initialized
            || bw        != m_cBW
            || bh        != m_cBH
            || useGlow   != m_cUseGlow
            || glowSz    != m_cGlowSz
            || glowAlpha != m_cGlowAlpha;

        if (settingsChanged) {
            m_cBW        = bw;
            m_cBH        = bh;
            m_cUseGlow   = useGlow;
            m_cGlowSz    = glowSz;
            m_cGlowAlpha = glowAlpha;
            m_initialized = true;
            m_goingUp    = goingUp;
            this->rebuild(bw, bh, useGlow, glowSz, glowAlpha);
        } else if (m_goingUp != goingUp) {
            // Solo cambió la dirección: actualizamos colores sin reconstruir
            m_goingUp = goingUp;
            this->updateColors();
        }
    }

private:
    // Amarillo subiendo, azul bajando
    ccColor3B barColor() const {
        return m_goingUp
            ? ccc3(255, 215, 0)    // Amarillo dorado
            : ccc3(0,   120, 255); // Azul eléctrico
    }

    // Reconstruye todos los nodos visuales
    void rebuild(float bw, float bh, bool useGlow, float glowSz, int glowAlpha) {
        this->removeAllChildren();
        for (auto& ptr : m_glowLayers) ptr = nullptr;
        m_bar = nullptr;

        auto col = barColor();

        if (useGlow) {
            for (int i = 0; i < GLOW_STEPS; i++) {
                // i=0 → capa más exterior / más transparente
                // i=GLOW_STEPS-1 → capa más interior / más opaca
                float f  = static_cast<float>(GLOW_STEPS - i) / GLOW_STEPS;
                float ew = bw + glowSz * 2.0f * f;
                float eh = bh + glowSz * 2.0f * f;
                // Alpha decrece hacia afuera para simular caída de luminosidad
                auto a = static_cast<GLubyte>(glowAlpha * (1.0f - f * 0.80f));

                auto* layer = CCLayerColor::create(
                    ccc4(col.r, col.g, col.b, a), ew, eh
                );
                layer->setPosition(-ew * 0.5f, -eh * 0.5f);
                // Blending aditivo: las capas se suman → efecto glow real
                layer->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                this->addChild(layer, i);
                m_glowLayers[i] = layer;
            }
        }

        // Barra sólida encima de todo el glow
        m_bar = CCLayerColor::create(ccc4(col.r, col.g, col.b, 255), bw, bh);
        m_bar->setPosition(-bw * 0.5f, -bh * 0.5f);
        this->addChild(m_bar, GLOW_STEPS);
    }

    // Actualización barata: solo cambia colores, sin tocar la estructura
    void updateColors() {
        auto col = barColor();
        if (m_bar) m_bar->setColor(col);
        for (auto* layer : m_glowLayers) {
            if (layer) layer->setColor(col);
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  Hook de PlayLayer
// ─────────────────────────────────────────────────────────────────────────────
class $modify(DI_PlayLayer, PlayLayer) {
    struct Fields {
        DirectionBar* bar = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto* bar = DirectionBar::create();
        if (!bar) return true; // No fatal; el juego sigue funcionando

        m_fields->bar = bar;
        bar->setDirection(false);  // Estado inicial: bajando → azul
        bar->syncPosition();

        // m_uiLayer es la capa HUD fija; no se mueve con la cámara del nivel
        if (this->m_uiLayer) {
            this->m_uiLayer->addChild(bar, 100);
        }

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto* bar = m_fields->bar;
        if (!bar || !m_player1 || m_player1->m_isDead) return;

        // Velocidad Y positiva en Cocos2d = movimiento hacia arriba
        bool goingUp = m_player1->m_yVelocity > 0.0f;

        bar->setDirection(goingUp);
        bar->syncPosition();
    }

    void onQuit() {
        // Evitar puntero colgante si se llamara update tras salir
        m_fields->bar = nullptr;
        PlayLayer::onQuit();
    }
};
