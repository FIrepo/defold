#ifndef DM_ENGINE_PRIVATE_H
#define DM_ENGINE_PRIVATE_H

#include <stdint.h>

#include <dlib/configfile.h>
#include <dlib/hashtable.h>
#include <dlib/message.h>

#include <resource/resource.h>

#include <render/render.h>
#include <render/font_renderer.h>
#include <rig/rig.h>

#include <hid/hid.h>
#include <input/input.h>

#include <gameobject/gameobject.h>

#include <gui/gui.h>

#include <gamesys/gamesys.h>

#include <record/record.h>

#include "engine_service.h"
#include "engine_ddf.h"
#include "engine.h"

namespace dmEngine
{
    const uint32_t MAX_RUN_RESULT_ARGS = 32;
    struct RunResult
    {
        RunResult()
        {
            memset(this, 0, sizeof(*this));
        }

        //`RunResult::EXIT` value (as `run_action` in `AppDelegate.m` `ShutdownEngine()`)
        // compares with GLFW_APP_RUN_EXIT, that's why `RunResult` should have the same values as `glfwAppRunAction`
        enum Action
        {
            NONE,
            EXIT = -1,
            REBOOT = 1,
        };

        void Free()
        {
            for (uint32_t i = 0; i < MAX_RUN_RESULT_ARGS; ++i)
            {
                if (m_Argv[i])
                {
                    free(m_Argv[i]);
                }
            }
        }

        int     m_Argc;
        char*   m_Argv[MAX_RUN_RESULT_ARGS];
        int32_t m_ExitCode;
        Action  m_Action;
    };

    struct Stats
    {
        Stats();

        uint32_t m_FrameCount;
    };

    struct RecordData
    {
        RecordData()
        {
            memset(this, 0, sizeof(*this));
        }

        dmRecord::HRecorder m_Recorder;
        char*               m_Buffer;
        uint32_t            m_FrameCount;
        uint32_t            m_FramePeriod;
        uint32_t            m_Fps;
    };

    enum Vsync
    {
        VSYNC_SOFTWARE = 0,
        VSYNC_HARDWARE = 1,

    };

    struct Engine
    {
        Engine(dmEngineService::HEngineService engine_service);
        dmEngineService::HEngineService             m_EngineService;
        dmConfigFile::HConfig                       m_Config;

        RunResult                                   m_RunResult;
        bool                                        m_Alive;

        dmGameObject::HRegister                     m_Register;
        dmGameObject::HCollection                   m_MainCollection;
        dmArray<dmGameObject::InputAction>          m_InputBuffer;

        uint32_t                                    m_LastReloadMTime;

        float                                       m_MouseSensitivity;

        dmGraphics::HContext                        m_GraphicsContext;
        dmRender::HRenderContext                    m_RenderContext;
        dmGameSystem::PhysicsContext                m_PhysicsContext;
        dmGameSystem::ParticleFXContext             m_ParticleFXContext;
        /// If the shared context is set, the three environment specific contexts below will point to the same context
        dmScript::HContext                          m_SharedScriptContext;
        dmScript::HContext                          m_GOScriptContext;
        dmScript::HContext                          m_RenderScriptContext;
        dmScript::HContext                          m_GuiScriptContext;
        dmResource::HFactory                        m_Factory;
        dmGameSystem::GuiContext                    m_GuiContext;
        dmMessage::HSocket                          m_SystemSocket;
        dmGameSystem::SpriteContext                 m_SpriteContext;
        dmGameSystem::CollectionProxyContext        m_CollectionProxyContext;
        dmGameSystem::FactoryContext                m_FactoryContext;
        dmGameSystem::CollectionFactoryContext      m_CollectionFactoryContext;
        dmGameSystem::SpineModelContext             m_SpineModelContext;
        dmGameSystem::ModelContext                  m_ModelContext;
        dmGameSystem::LabelContext                  m_LabelContext;
        dmGameSystem::TilemapContext                m_TilemapContext;
        dmGameObject::ModuleContext                 m_ModuleContext;

        dmRender::HFontMap                          m_SystemFontMap;
        dmHID::HContext                             m_HidContext;
        dmInput::HContext                           m_InputContext;
        dmInput::HBinding                           m_GameInputBinding;
        dmRender::HDisplayProfiles                  m_DisplayProfiles;

        dmGameSystem::RenderScriptPrototype*        m_RenderScriptPrototype;

        Stats                                       m_Stats;

        bool                                        m_UseSwVsync;
        bool                                        m_UseVariableDt;
        bool                                        m_WasIconified;
        bool                                        m_QuitOnEsc;
        bool                                        m_ConnectionAppMode;        //!< If the app was started on a device, listening for connections
        bool                                        m_RunWhileIconified;
        uint64_t                                    m_PreviousFrameTime;
        uint64_t                                    m_PreviousRenderTime;
        uint64_t                                    m_FlipTime;
        uint32_t                                    m_UpdateFrequency;
        uint32_t                                    m_Width;
        uint32_t                                    m_Height;
        uint32_t                                    m_ClearColor;
        float                                       m_InvPhysicalWidth;
        float                                       m_InvPhysicalHeight;
        Vsync                                       m_VsyncMode;

        RecordData                                  m_RecordData;
    };


    HEngine New(dmEngineService::HEngineService engine_service);
    void Delete(HEngine engine);
    bool Init(HEngine engine, int argc, char *argv[]);
    void Step(HEngine engine);

    void ReloadResources(HEngine engine, const char* extension);
    bool LoadBootstrapContent(HEngine engine, dmConfigFile::HConfig config);
    void UnloadBootstrapContent(HEngine engine);



    // Creates and initializes the engine. Returns the engine instance
    typedef void* (*EngineCreate)(int argc, char** argv);
    // Destroys the engine instance after finalizing each system
    typedef void (*EngineDestroy)(void* engine);
    // Steps the engine 1 tick
    typedef dmEngineUpdateResult (*EngineUpdate)(void* engine);
    // Called before the destroy function
    typedef void (*EngineGetResult)(void* engine, int* run_action, int* exit_code, int* argc, char*** argv);

    struct RunLoopParams
    {
        int     m_Argc;
        char**  m_Argv;

        void*   m_AppCtx;
        void    (*m_AppCreate)(void* ctx);
        void    (*m_AppDestroy)(void* ctx);

        EngineCreate        m_EngineCreate;
        EngineDestroy       m_EngineDestroy;
        EngineUpdate        m_EngineUpdate;
        EngineGetResult     m_EngineGetResult;
    };

    /**
     *
     */
    int RunLoop(const RunLoopParams* params);
}

#endif // DM_ENGINE_PRIVATE_H
