#include "Ren/ECS/ComponentSystems.h"
#include "Ren/ECS/Scene.h"
#include "Ren/ECS/NativeScript.h"
#include "Ren/Renderer/Renderer.h"

using namespace Ren;

#pragma region --> Render system

void RenderSystem::Render()
{
    // Render all sprites.
    auto view = m_scene->SceneView<TransformComponent, SpriteComponent>();
    for (auto&& ent : view)
    {
        auto [trans, sprite] = view.get(ent);
        Renderer::SetRenderLayer(trans.layer);
        Renderer::SubmitQuad(trans.position, trans.scale, trans.rotation, sprite.color, sprite.GetTexture());
    }
}

#pragma endregion
#pragma region --> NativeScript system

// Helper function to iterate through all entities, that have bound script.
template<typename Func>
constexpr void for_each_script(Scene* scene, Func func)
{
    auto view = scene->m_Registry->view<NativeScriptComponent>();
    for (auto&& ent : view)
    {
        auto [script] = view.get(ent);
        if (script.script_instance)
            func(ent, script.script_instance);
    }
}

inline void unbind_script(entt::registry& reg, entt::entity ent) { reg.get<NativeScriptComponent>(ent).Unbind(); }

void NativeScriptSystem::Init()
{
    m_scene->m_Registry->on_destroy<NativeScriptComponent>().connect<&unbind_script>();

    for_each_script(m_scene, [this](entt::entity ent, NativeScript* script) {
        script->m_entity = Entity{ ent, this->m_scene };
        script->m_input = this->m_input;
        script->OnInit();
    });
}
void NativeScriptSystem::Destroy()
{
    for_each_script(m_scene, [this](entt::entity ent, NativeScript* script) {
        script->OnDestroy();
    });
}
void NativeScriptSystem::Update(float dt)
{
    for_each_script(m_scene, [&dt](entt::entity ent, NativeScript* script) {
        script->OnUpdate(dt);
    });
}

#pragma endregion