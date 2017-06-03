#include "../pcheader.h"

#include "UIBase.h"

#include "../customs/CustomTrayManager.h"

Ogre::PanelOverlayElement* UIBase::createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material)
{
    Ogre::PanelOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::PanelOverlayElement*)om.createOverlayElement("Panel", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setMaterialName(material);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    mCreatedElements.push_back(res);

    return res;
}

Ogre::PanelOverlayElement* UIBase::createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material)
{
    return createPanel(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y, material);
}

Ogre::TextAreaOverlayElement* UIBase::createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    Ogre::TextAreaOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    mCreatedElements.push_back(res);

    return res;
}

OgreBites::Button* UIBase::createButton(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width)
{
    OgreBites::Button* res = trayMgr->createButton(trayLoc, name, caption, width);

    mCreatedWidgets.push_back(res);

    return res;
}

OgreBites::TextBox* UIBase::createTextBox(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width, Ogre::Real height)
{
    OgreBites::TextBox* res = trayMgr->createTextBox(trayLoc, name, caption, width, height);

    mCreatedWidgets.push_back(res);

    return res;
}

OgreBites::SelectMenu* UIBase::createSelectMenu(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width, unsigned int maxItemsShown, const Ogre::StringVector& items)
{
    OgreBites::SelectMenu* res = trayMgr->createThickSelectMenu(trayLoc, name, caption, width, maxItemsShown, items);

    mCreatedWidgets.push_back(res);

    return res;
}

void UIBase::destroy(CustomTrayManager* trayMgr)
{
    trayMgr->setListener(NULL);

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

    for(size_t q = 0; q < mCreatedElements.size(); ++q)
        om.destroyOverlayElement(mCreatedElements[q]);

    for(size_t q = 0; q < mCreatedWidgets.size(); ++q)
        trayMgr->destroyWidget(mCreatedWidgets[q]);

    mCreatedElements.clear();
    mCreatedWidgets.clear();
}