//
// Created by fang on 2023/1/10.
//

#include "fragment/player_danmaku_setting.hpp"
#include "view/danmaku_core.hpp"
#include "view/button_close.hpp"
#include "utils/config_helper.hpp"

using namespace brls::literals;

PlayerDanmakuSetting::PlayerDanmakuSetting() {
    this->inflateFromXMLRes("xml/fragment/player_danmaku_setting.xml");
    brls::Logger::debug("Fragment PlayerDanmakuSetting: create");

    this->registerAction("hints/cancel"_i18n, brls::BUTTON_B, [](...) {
        brls::Application::popActivity();
        return true;
    });

    closebtn->registerClickAction([](...) {
        brls::Application::popActivity();
        return true;
    });

    auto& conf = ProgramConfig::instance();

    this->cellTop->init("wiliwili/player/danmaku/filter/top"_i18n,
                        DanmakuCore::DANMAKU_FILTER_SHOW_TOP, [](bool data) {
                            DanmakuCore::DANMAKU_FILTER_SHOW_TOP = data;
                            DanmakuCore::save();
                            DanmakuCore::instance().refresh();
                            return true;
                        });
    this->cellBottom->init("wiliwili/player/danmaku/filter/bottom"_i18n,
                           DanmakuCore::DANMAKU_FILTER_SHOW_BOTTOM,
                           [](bool data) {
                               DanmakuCore::DANMAKU_FILTER_SHOW_BOTTOM = data;
                               DanmakuCore::save();
                               DanmakuCore::instance().refresh();
                               return true;
                           });
    this->cellScroll->init("wiliwili/player/danmaku/filter/scroll"_i18n,
                           DanmakuCore::DANMAKU_FILTER_SHOW_SCROLL,
                           [](bool data) {
                               DanmakuCore::DANMAKU_FILTER_SHOW_SCROLL = data;
                               DanmakuCore::save();
                               DanmakuCore::instance().refresh();
                               return true;
                           });
    this->cellColor->init("wiliwili/player/danmaku/filter/color"_i18n,
                          DanmakuCore::DANMAKU_FILTER_SHOW_COLOR,
                          [](bool data) {
                              DanmakuCore::DANMAKU_FILTER_SHOW_COLOR = data;
                              DanmakuCore::save();
                              DanmakuCore::instance().refresh();
                              return true;
                          });

    std::vector<std::string> levels;
    for (size_t i = 1; i <= 10; i++)
        levels.emplace_back(
            fmt::format("wiliwili/player/danmaku/filter/level_n"_i18n, i));
    this->cellLevel->init(
        "wiliwili/player/danmaku/filter/level"_i18n, levels,
        conf.getIntOptionIndex(SettingItem::DANMAKU_FILTER_LEVEL),
        [](int data) {
            DanmakuCore::DANMAKU_FILTER_LEVEL = data + 1;
            DanmakuCore::save();
            DanmakuCore::instance().refresh();
            return true;
        });

    this->cellArea->init(
        "wiliwili/player/danmaku/style/area"_i18n,
        {"wiliwili/player/danmaku/style/area_1_4"_i18n,
         "wiliwili/player/danmaku/style/area_2_4"_i18n,
         "wiliwili/player/danmaku/style/area_3_4"_i18n,
         "wiliwili/player/danmaku/style/area_4_4"_i18n},
        conf.getIntOptionIndex(SettingItem::DANMAKU_STYLE_AREA), [](int data) {
            DanmakuCore::DANMAKU_STYLE_AREA = 25 + data * 25;
            DanmakuCore::instance().refresh();
            return true;
        });

    auto alpha = ProgramConfig::instance().getOptionData(
        SettingItem::DANMAKU_STYLE_ALPHA);
    this->cellAlpha->init(
        "wiliwili/player/danmaku/style/alpha"_i18n, alpha.optionList,
        conf.getIntOptionIndex(SettingItem::DANMAKU_STYLE_ALPHA),
        [alpha](int data) {
            DanmakuCore::DANMAKU_STYLE_ALPHA = alpha.rawOptionList[data];
            DanmakuCore::save();
            DanmakuCore::instance().refresh();
            return true;
        });

    auto font = ProgramConfig::instance().getOptionData(
        SettingItem::DANMAKU_STYLE_FONTSIZE);
    this->cellFontsize->init(
        "wiliwili/player/danmaku/style/fontsize"_i18n, font.optionList,
        conf.getIntOptionIndex(SettingItem::DANMAKU_STYLE_FONTSIZE),
        [font](int data) {
            DanmakuCore::DANMAKU_STYLE_FONTSIZE = font.rawOptionList[data];
            DanmakuCore::save();
            DanmakuCore::instance().refresh();
            return true;
        });

    auto speed = ProgramConfig::instance().getOptionData(
        SettingItem::DANMAKU_STYLE_SPEED);
    this->cellSpeed->init(
        "wiliwili/player/danmaku/style/speed"_i18n,
        {
            "wiliwili/player/danmaku/style/speed_slow_plus"_i18n,
            "wiliwili/player/danmaku/style/speed_slow"_i18n,
            "wiliwili/player/danmaku/style/speed_moderate"_i18n,
            "wiliwili/player/danmaku/style/speed_fast"_i18n,
            "wiliwili/player/danmaku/style/speed_fast_plus"_i18n,
        },
        conf.getIntOptionIndex(SettingItem::DANMAKU_STYLE_SPEED),
        [speed](int data) {
            DanmakuCore::DANMAKU_STYLE_SPEED = speed.rawOptionList[data];
            DanmakuCore::save();
            DanmakuCore::instance().refresh();
            return true;
        });
}

PlayerDanmakuSetting::~PlayerDanmakuSetting() {
    brls::Logger::debug("Fragment PlayerDanmakuSetting: delete");
}

brls::View* PlayerDanmakuSetting::create() {
    return new PlayerDanmakuSetting();
}

bool PlayerDanmakuSetting::isTranslucent() { return true; }

brls::View* PlayerDanmakuSetting::getDefaultFocus() {
    return this->settings->getDefaultFocus();
}