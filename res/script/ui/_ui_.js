


require("res/script/ui/BasePanel.js");
require("res/script/ui/LoginPanel.js");
require("res/script/ui/ConsolePanel.js");
require("res/script/ui/PanelMgr.js");


gn.ui = {
    entrance:{
        file:'res/ui/Entrance.json',
        inst:new EntrancePanel()
    },
    login:{
        file:'res/ui/LoginPanel.json',
        inst:new LoginPanel()
    },
    console:{
        file:'res/ui/Console.json',
        inst:new ConsolePanel()
    }
};

