#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Loader.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

class LevelInfo {
public:
    int id;
    std::string name;
    std::string author;
    int position;
    std::string video;
};

std::vector<LevelInfo> g_list = {};

void loadListFromGitHub() {
    // Metti qui il link raw del tuo list.json su GitHub
    std::string url = "https://raw.githubusercontent.com/tuonome/GD-Italian-List/main/list.json";
    
    web::WebRequest request = web::WebRequest();
    request.get(url).listen([](web::WebResponse* res) {
        if (!res->ok()) {
            log::error("Errore nel download della lista: {}", res->code());
            return;
        }

        auto json = matjson::parse(res->string().unwrap()).unwrap();
        g_list.clear();

        for (auto& item : json.as_array().unwrap()) {
            LevelInfo lvl;
            lvl.id = item["id"].as_int().unwrap();
            lvl.name = item["name"].as_string().unwrap();
            lvl.author = item["author"].as_string().unwrap();
            lvl.position = item["position"].as_int().unwrap();
            lvl.video = item["video"].as_string().unwrapOr("");
            g_list.push_back(lvl);
        }
        log::info("Lista caricata! {} livelli", g_list.size());
    });
}

// Esempio: aggiungo un bottone nel menu principale
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::create("listBtn.png"_spr), // metti un'icona in resources
            this,
            menu_selector(MyMenuLayer::onOpenList)
        );
        btn->setPosition(winSize.width - 50, 50);

        this->getChildByType<CCMenu>(0)->addChild(btn);
        return true;
    }

    void onOpenList(CCObject*) {
        FLAlertLayer::create(
            "Lista Italiana", 
            fmt::format("Caricati {} livelli dalla lista", g_list.size()), 
            "OK"
        )->show();
        
        // Qui poi apri la tua Layer custom con la lista vera
    }
};

// Carica la lista quando parte il gioco
$execute {
    loadListFromGitHub();
    log::info("Mod Lista Italiana caricata!");
}
