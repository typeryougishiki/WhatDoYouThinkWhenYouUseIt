// SFSE message listener, use this to do stuff at specific moments during runtime
#include"utils/Config.h"
#include"hooks/ActorEquipManager.h"
#include"hooks/SetFormEditorID.h"
#include"CsvManager.h"
#include"ThinkDatas.h"
void SfseMsgListener(SFSE::MessagingInterface::Message* message) noexcept{
    NoExceptField::Field([&message]() ->void {
        switch (message->type) {
        case SFSE::MessagingInterface::kPostLoad: {
            logger::info("Hello Starborn!");
            hook::ActorEquipManger::EquipObject::AMustFuncCallWithActorAndEquipObjectAsArgument::Install();
        }break;
        case SFSE::MessagingInterface::kPostPostLoad: {
        }break;
        case SFSE::MessagingInterface::kPostDataLoad: {
        }break;
        case SFSE::MessagingInterface::kPostPostDataLoad: {
        }break;
        default: {
        }break;
        }
        }
    );
}

inline void LoadThinkDatas() {
        auto csv_manager = CsvManagerSingleton::GetSingleton();
        bool has_final = false;
        for (const auto& path : csv_manager->GetConfiguredCsvNames()) {
            if (path == "final.csv"sv) {
                has_final = true;
                continue;
            }
            logger::info("Load thinks from {}"sv, path);
            csv_manager->ConstAccessCsv(path, [](const rapidcsv::Document& doc) {ThinkDatas::AddToThinkDatasSingleton(doc, false); });
        }
        if (has_final) {
            logger::info("Load thinks from final.csv"sv);
            csv_manager->ConstAccessCsv("final.csv"sv, [](const rapidcsv::Document& doc) {ThinkDatas::AddToThinkDatasSingleton(doc, true); });
        }
        ThinkDatas::Inited.store(true);
}
// Main SFSE plugin entry point, initialize everything here
SFSEPluginLoad(const SFSE::LoadInterface* sfse){
    SFSE::Init(sfse);
    return NoExceptField::FieldWithReturn<bool>([]() ->auto {
        Config::LoadSettings();
        LoadThinkDatas();
        logger::info("{} {} is loading...", Plugin::Name, Plugin::Version.string());
        if (const auto messaging{ SFSE::GetMessagingInterface() }; !messaging->RegisterListener(SfseMsgListener))
            return false;
        logger::info("{} has finished loading.", Plugin::Name);
        return true;
    }, false,"Init failed.");

}