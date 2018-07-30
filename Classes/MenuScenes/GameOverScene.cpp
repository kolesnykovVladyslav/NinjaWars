/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "GameOverScene.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "Definitions.h"

USING_NS_CC;

LEVEL lastLevel;
GAME_RESULT gameResult;
const std::string resultImage[2] = {"Victory.png", "Defeat.png"};

Scene* GameOverScene::createScene(LEVEL level, GAME_RESULT result)
{
    gameResult = result;
    lastLevel = level;
    return GameOverScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameOverScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto backgroundSprite = Sprite::create("MenuBG.png", Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    backgroundSprite->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(backgroundSprite);
    
    //image
    auto image = Sprite::create(resultImage[gameResult]);
    image->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.7 + origin.y));
    this->addChild(image);
    
    //menuitems
    auto retryItem = MenuItemFont::create("Retry", CC_CALLBACK_1(GameOverScene::goToGameScene, this));
    retryItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.45 + origin.y));
    retryItem->setColor(Color3B(0, 0, 0));
    retryItem->setFontSizeObj(42);
    auto menuItem = MenuItemFont::create("Menu", CC_CALLBACK_1(GameOverScene::goToMainMenuScene, this));
    menuItem->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height * 0.25 + origin.y));
    menuItem->setColor(Color3B(0, 0, 0));
    menuItem->setFontSizeObj(42);
    
    auto menu = Menu::create(retryItem, menuItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu);
    
    return true;
}

void GameOverScene::goToMainMenuScene(cocos2d::Ref *sender)
{
    auto scene = MainMenuScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(TRASNSITION_TIME, scene));
}

void GameOverScene::goToGameScene(cocos2d::Ref *sender)
{
    auto scene = GameScene::createScene(lastLevel);
    Director::getInstance()->replaceScene(TransitionFade::create(TRASNSITION_TIME, scene));
}
