#include "Performance.h"
#include <ll/api/memory/Hook.h>
#include "mc/server/ServerLevel.h"

server::Performance &server::Performance::getInstance() {
    static Performance instance;
    return instance;
}

LL_AUTO_TYPE_INSTANCE_HOOK(LevelTickEventHook, HookPriority::Normal, ServerLevel, &ServerLevel::$_subTick, void) {
    origin();
}
