

#include <string>

#include "common/Log.h"
#include "common/Performance.h"
#include "common/TimeSystem.h"
#include "app/Config.h"
#include "WarApp.h"
#include "ConfigPool.h"
#include "gfx/gfx.h"
#include "gfx/CocosApp.h"
#include "pawn/bevtree/BehaviorsAndPreconditions.h"
#include "ECS/ecs.h"
#include "entity/EntityCreators.h"
#include "skill/SkillManager.h"
#include "skill/BuffManager.h"

#include "scripting/js-bindings/manual/ScriptingCore.h"



CocosApp cocosApp;

USING_NS_CC;
using namespace Genius;


bool WarApp::Init()
{
	do
	{
		cocosApp.PostRun();

		if (cfg_EnableConsole)
		{
			AllocConsole();
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);
		}

		Log::Info("WarApp::Init Begin...");

		Performance::Begin();

		TimeSystem::GetSingleton()->Init();
		cfg::ConfigPool::GetSingleton()->Init();
		SceneManager::GetSingleton()->Init();
		SystemStatusPanel::GetSingleton()->Init();
		AI::BHTree::BehaviorLoader::GetSingleton()->Init(gBehaviorMap, gPreconditionMap);
		ECSWorld::GetSingleton()->GetSystemManager()->Init();

		cocosApp.RunScript();

		AudioManager::GetSingleton()->PlayBGM("res/audio/main_theme.mp3");

		float dt = Performance::End();
		Log::Info("WarApp::Init End. Cost : %f s", dt);

	} while (0);

	return true;
}

void WarApp::Tick(float time)
{
	float tickSec = time;
	TimeSystem::GetSingleton()->Tick(tickSec);
	SystemStatusPanel::GetSingleton()->ClearStatus();
	
	ECSWorld::GetSingleton()->LoopStart();
	ECSWorld::GetSingleton()->SetDeltaTime(tickSec);
	ECSWorld::GetSingleton()->Process();

	SkillManager::GetSingleton()->Update(tickSec);
	BuffManager::GetSingleton()->Update(tickSec);

	SceneManager::GetSingleton()->Update();

	cocosApp.StepRun();

	_Update_Fast(tickSec);
	_Update_Normal(tickSec);
	_Update_Slow(tickSec);

}

void WarApp::Destroy()
{
	Genius::EntityManager * em = ECSWorld::GetSingleton()->GetEntityManager();
	em->RemoveAllEntities();

	AI::BHTree::BehaviorLoader::GetSingleton()->Destroy();
	cfg::ConfigPool::GetSingleton()->Destroy();

	cocosApp.AfterRun();
	
	AudioManager::GetSingleton()->Release();

	if (cfg_EnableConsole)
	{
		FreeConsole();
	}
}

void WarApp::_Update_Fast(float time)
{

}

void WarApp::_Update_Normal(float time)
{
	static float timePassed = 0;
	timePassed += time;
	if (timePassed < 0.2f)
		return;

	char jsCallString[32];
	sprintf_s(jsCallString, "Game.OnUpdate(%f);", timePassed);
	ScriptingCore::getInstance()->evalString(jsCallString);
	
	timePassed = 0;
}

void WarApp::_Update_Slow(float time)
{
	static float timePassed = 0;
	timePassed += time;
	if (timePassed < 1.0f)
		return;

	char jsCallString[32];
	sprintf_s(jsCallString, "Game.OnLongUpdate(%f);", timePassed);
	ScriptingCore::getInstance()->evalString(jsCallString);

	timePassed = 0;
}

