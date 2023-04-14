//
// Created by fang on 2022/7/18.
//

#include "view/video_comment.hpp"
#include "view/text_box.hpp"
#include "view/svg_image.hpp"
#include "utils/number_helper.hpp"
#include "bilibili.h"

using namespace brls::literals;

VideoComment::VideoComment() {
    brls::Logger::verbose("View VideoComment: create");
    this->inflateFromXMLRes("xml/views/video_comment.xml");

    this->registerColorXMLAttribute("mainTextColor", [this](NVGcolor value) {
        this->setMainTextColor(value);
    });

    this->registerFloatXMLAttribute("maxRows", [this](float value) {
        this->commentContent->setMaxRows((int)value);
    });
}

VideoComment::~VideoComment() {
    brls::Logger::verbose("View VideoComment: delete");
}

RecyclingGridItem* VideoComment::create() { return new VideoComment(); }

void VideoComment::setMainTextColor(NVGcolor color) {
    this->commentContent->setTextColor(color);
    this->userInfo->setMainTextColor(color);
}

void VideoComment::setData(bilibili::VideoCommentResult data) {
    this->comment_data = data;

    std::string subtitle = wiliwili::sec2date(data.ctime);
    if (!data.reply_control.location.empty()) {
        subtitle += "  " + data.reply_control.location;
    }

    RichTextData d;
    const std::string& msg = data.content.message;
    const auto& emote      = data.content.emote;
    NVGcolor textColor     = brls::Application::getTheme()["brls/text"];
    size_t start = 0, index = -1;
    for (size_t i = 0; i < msg.length(); i++) {
        if (msg[i] == '[') {
            index = i;
        } else if (msg[i] == ']') {
            // 没找到左半边中括号
            if (index == -1) continue;
            // 没找到表情包
            std::string key = msg.substr(index, i - index + 1);
            if (!emote.count(key)) continue;
            // 找到表情包
            // 文字分段
            if (index > start)
                d.emplace_back(std::make_shared<RichTextSpan>(
                    msg.substr(start, index - start), textColor));
            // 表情包
            if (emote.at(key).size == 2) {
                d.emplace_back(std::make_shared<RichTextImage>(
                    emote.at(key).url + ImageHelper::emoji_size2_ext, 40, 40));
            } else {
                d.emplace_back(std::make_shared<RichTextImage>(
                    emote.at(key).url + ImageHelper::emoji_size1_ext, 30, 30));
            }

            start = i + 1;
            index = -1;
        }
    }
    if (start < msg.length()) {
        d.emplace_back(std::make_shared<RichTextSpan>(
            msg.substr(start, msg.length() - start), textColor));
    }

    // 比较丑陋地简单实现笔记图片
    if (!data.content.pictures.empty())
        d.emplace_back(std::make_shared<RichTextSpan>("\n\n\n", textColor));
    for (size_t i = 0; i < data.content.pictures.size(); i++) {
        d.emplace_back(std::make_shared<RichTextImage>(
            data.content.pictures[i].img_src + ImageHelper::note_ext, 96, 96));
        if ((i + 1) % 3 == 0 && i != data.content.pictures.size() - 1) {
            d.emplace_back(
                std::make_shared<RichTextSpan>("\n\n\n\n", textColor));
        }
    }
    if (!data.content.pictures.empty())
        d.emplace_back(std::make_shared<RichTextSpan>("\n\n", textColor));

    // 设置富文本
    this->commentContent->setRichText(d);

    this->userInfo->setUserInfo(data.member.avatar + ImageHelper::face_ext,
                                data.member.uname, subtitle);

    int lv = data.member.level_info.current_level;
    if (lv < 0 || lv > 6) {
        this->userLevel->setVisibility(brls::Visibility::GONE);
    } else {
        this->userLevel->setVisibility(brls::Visibility::VISIBLE);
        if (data.member.is_uploader) {
            this->userLevel->setImageFromSVGRes(
                fmt::format("svg/user-up.svg", lv));
        } else if (lv == 6 && data.member.is_senior_member) {
            this->userLevel->setImageFromSVGRes("svg/user-lv6p.svg");
        } else {
            this->userLevel->setImageFromSVGRes(
                fmt::format("svg/user-lv{}.svg", lv));
        }
    }

    if (data.action) {
        this->svgLike->setImageFromSVGRes("svg/comment-agree-active.svg");
    } else {
        this->svgLike->setImageFromSVGRes("svg/comment-agree-grey.svg");
    }

    this->labelLike->setText(wiliwili::num2w(data.like));
    this->labelReply->setText(wiliwili::num2w(data.rcount));
}

void VideoComment::prepareForReuse() {
    this->userInfo->getAvatar()->setImageFromRes("pictures/default_avatar.png");
}

void VideoComment::hideReplyIcon(bool hide) {
    if (hide) {
        this->labelReply->setVisibility(brls::Visibility::GONE);
        this->svgReply->setVisibility(brls::Visibility::GONE);
    } else {
        this->svgReply->setVisibility(brls::Visibility::VISIBLE);
        this->labelReply->setVisibility(brls::Visibility::VISIBLE);
    }
}

void VideoComment::cacheForReuse() {
    ImageHelper::clear(this->userInfo->getAvatar());
}

/// GridHintView

GridHintView::GridHintView() {
    this->setFocusable(false);
    hintLabel = new brls::Label();
    hintLabel->setFontSize(16);
    hintLabel->setMarginLeft(8);
    hintLabel->setTextColor(
        brls::Application::getTheme().getColor("font/grey"));
    this->addView(hintLabel);
}

RecyclingGridItem* GridHintView::create() { return new GridHintView(); }

/// VideoCommentReply

VideoCommentReply::VideoCommentReply() {
    auto theme = brls::Application::getTheme();
    this->setFocusable(true);
    this->setHeight(40);
    this->setJustifyContent(brls::JustifyContent::FLEX_START);
    this->setCornerRadius(8);
    this->setBackgroundColor(theme.getColor("color/grey_2"));
    hintLabel = new brls::Label();
    this->hintLabel->setMarginLeft(20);
    this->hintLabel->setFontSize(18);
    this->hintLabel->setText("wiliwili/player/single_comment/hint"_i18n);
    this->hintLabel->setTextColor(theme.getColor("font/grey"));
    this->addView(hintLabel);
}

RecyclingGridItem* VideoCommentReply::create() {
    return new VideoCommentReply();
}