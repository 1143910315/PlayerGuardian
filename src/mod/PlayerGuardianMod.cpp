#include "command/UICommand.h"
#include "PlayerGuardianMod.h"
#include <ll/api/mod/RegisterHelper.h>

namespace PlayerGuardian {
    PlayerGuardianMod& PlayerGuardianMod::getInstance() {
        static PlayerGuardianMod instance;
        return instance;
    }

    bool PlayerGuardianMod::load() {
        getSelf().getLogger().debug("Loading...");
        webuiServer::WebUI::getInstance().startServer("http://localhost:5173/", 9000);
        // Code for loading the mod goes here.
        return true;
    }

    bool PlayerGuardianMod::enable() {
        getSelf().getLogger().debug("Enabling...");
        command::UICommand::getInstance().registerCommand();
        // Code for enabling the mod goes here.
        return true;
    }

    bool PlayerGuardianMod::disable() {
        getSelf().getLogger().debug("Disabling...");
        // Code for disabling the mod goes here.
        return true;
    }

    bool PlayerGuardianMod::unload() {
        webuiServer::WebUI::getInstance().stopServer();
        return true;
    }
} // namespace PlayerGuardian

LL_REGISTER_MOD(PlayerGuardian::PlayerGuardianMod, PlayerGuardian::PlayerGuardianMod::getInstance());
