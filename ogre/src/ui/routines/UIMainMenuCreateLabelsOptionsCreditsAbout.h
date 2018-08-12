void UIMainMenuLabels::createLabelsOptionsCreditsAbout(const Ogre::Matrix4& screenAdaptionRelative)
{
    const Ogre::Real viewportHeight = screenAdaptionRelative[1][1] * 480.0f;
    const Ogre::Real viewportWidth = screenAdaptionRelative[0][0] * 640.0f;

    //credits
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(10.0f, 10.0f, 0.0f, 0.0f);;
            UILabel* label = mUILabelsManager.add("mCredits");
            label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            label->setFixed(true);
            label->getTextArea()->setCaption("");
            label->getTextArea()->setCharHeight(16.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setSpaceWidth(9.0f);
            label->getTextArea()->setHeight(16.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
            getMainBackground()->addChild(label->getTextArea());

            const Ogre::Real rightBoundary = viewportWidth / 2.0f - label->getTextArea()->getLeft() - viewportWidth / 50.0f;

            std::string desc = "This game is dedicated to Nada & Michael Harrison, Colleen & Trevor Siegele, without whose support Powerslide would never have been made. Jim Perkins, Mike Bartholemew, Todd Colletti and Dave Adams whose confidence in Ratbag and Powerslide inspired many.\n\n"\
                "Project Manager: Greg Siegele\n\n"\
                "Technical Manager: Richard Harrison\n\n"\
                "Programmers: Richard Harrison, James Grieve, Jeffrey Lim, Saxon Druce, Garth Denley, Aaron Foo, Jim Rehn, Sam Yates, Adam Batters, Chris Carthew\n\n"\
                "Graphic Artists: Nick Young, Gavin Klose, Szymon Mienik, Glenn Davidson, Ryan Grobins, Jay Weston, David Short, Andrew Francis, Paul Murray, Damien Murray, Damien Thaller\n\n"\
                "Music and Sound Effects: Ashley Klose, Andre Pang\n\n"\
                "Writing: Paul Cummins, Sal Humphreys\n\n"\
                "Quality Assurance: Adam Batters, Shane Davis, David Todd, Richard Weaver, and Jeff Bekin, Michael Braggs, Stephen Braggs, Rob Brice, Tom Crago, Basil Genimahaliotis, Zoran Glavach, Ben Lehman, Zoe Lim, Michal Mienik, Daniel Venning, John Weston, Craig Williams.\n\n"\
                "Thanks to: Absolute Quality; AIMIA; AME, Martin Cooper and Liz Rankin; Arthur Andersen: Chris Sharpley, Rachelle Summerton, Kristian Moliere; Adam Lancman and Alfred Milgrom at Beam; Austrade: Nigel Warren, Bob Hunt, Seiichi Saito, Roo Takagi; Brendan McNamara; Brian Bruning; Bruce Bishop, Don Stephens; Col Stone; Dale Roberts; David Ward & Ray McIntosh; Everyone at Singletrac; Fusion Design; Interact; Jessica Young.\n\n"\
                "More thanks to: Mark Minarelli; Mark Standen; Mark Robertson, Mark Altschwager; Mike Burfield; Milt Barlow; Ngapartji; Paul Wyk; Peter Stobart; Rising Sun Pictures: Wayne Lewis, Gail Fuller, Tony Clark, Steve Roberts; SGI: Dave Savage, John Cranwell, Jon Noonan; Simon Cox; The Business Centre: Susan Andrews, Kevin O'Callaghan; The Digital Precinct, John Hunt; Xenotech, Bob Baker, Phil Harman.\n\n"\
                "Thanks to all the people at GT who helped make this project a reality: Aaron Younger; Alan Lewis; Cecile Borzakian; Chris Mollo; Doug Dyer; Edward Fong; Eric Pouzet; Gavin Rundle; Graeme Boxall; Harry Rubin; Ilja Doert; Julian White; Martin Good; Mary Gerardi; Matt Broughton; Matt Woodley; Max Taylor; Mike Calleja; Mike Ryder; Paul Fox; Reinhard Nuernberg; Shari Bernstein; Sharon Simmons, Tony Kee; Jesse Smith and Absolute Quality for their rigorous QA.\n\n"\
                "Extra special thanks to our partners who supported the project: Kate Soininen, Natasha Siemelink, Annette Plant, Daniela Catalano, Francesca Pinneri, Lani Davidson.\n\n"\
                "And thanks to everyone else who helped in our quest over the last four years.";

            wrapText(
                desc,
                rightBoundary,
                (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(label->getTextArea()->getFontName()).getPointer(),
                label->getTextArea()->getSpaceWidth(),
                label->getTextArea()->getCharHeight());

            label->getTextArea()->setCaption(desc);
        }
    }

    //about
    {
        {
            Ogre::Vector4 textBoxPos = screenAdaptionRelative * Ogre::Vector4(10.0f, 10.0f, 0.0f, 0.0f);;
            UILabel* label = mUILabelsManager.add("mAbout");
            label->init(0.0f, 0.0f, textBoxPos.x, textBoxPos.y);
            label->setFixed(true);
            label->getTextArea()->setCaption("");
            label->getTextArea()->setCharHeight(17.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setSpaceWidth(9.0f);
            label->getTextArea()->setHeight(17.0f * viewportHeight / 1024.0f);
            label->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Left);
            getMainBackground()->addChild(label->getTextArea());

            const Ogre::Real rightBoundary = viewportWidth / 2.0f - label->getTextArea()->getLeft() - viewportWidth / 50.0f;

            std::string desc = "The story\n\n"\
                "Early in the 21st century, the ozone layer disintegrated. Surprise! As the price of sunscreen skyrocketed, any species not turning a profit in the global marketplace soon found themselves unable to pay for UV protection. Bees, flies, and all the other creepy - crawlies got their sensitive eyes fried. No bees, no pollination. No pollination, no crops. No crops, starving humans. Starving humans, global war. It would have made a great movie, but the entertainment market is in recession, owing to the 99% decrease in consumers.\n\n"\
                "Anyway, after all the famines, fighting and nuclear fallout, after six billion people bowed out of the human comedy, what was left of humanity's dross and scum clung tenaciously to life on the earth's scorched surface.\n\n"\
                "Underground, away from the burning sun, wealthy corporations formed huge cities and began a new existence. Like human termites, they now live in totalitarian conditions, each performing their role in the hive. Comfortable but caged, these corporations have a very middle - class obsession with cleanliness and order, almost as if to deny the calamity that they caused above ground. On the surface, feral and feudal clans slave, sweat, fry and die under the ultra - violet radiation. Unaffectionately referred to as 'upside fry - babies' by the Corporates, they barely manage to eke out their barbarous existence, imperiled by pollution, pestilence and plague.\n\n"\
                "The decimation of billions of people is tragic, but the cars they left behind are an entertainment opportunity too good to refuse. Now, lawless gangs of feral joyriders race through deserted streets and countryside, docklands, sewers, and multi - level parking lots. Slapping together grunt machines of awesome power, they have become known as Powersliders.\n\n"\
                "Until recently, Powersliding was an illicit venture for an abusive and exclusive bunch of psychotic motor lunatics. Although it started as an exclusively feral pursuit, and banned by most Corporations, a bunch of Corporate rebel racers have recently joined the action and brought their radical new technology to the contest.\n\n"\
                "The Prizes\n\n"\
                "In a bitter irony, one of the only forms of vegetation to thrive in the ultraviolet ray - ravaged and hopelessly polluted landscape is the brussel sprout. Fruit has become a prize so rare that dozens have fought and died for the sake of a single bunch of bananas.\n\n"\
                "The glittering prize, the tantalizing impetus that inspires the drivers to feats of suicidal danger, is the obscenely rich trophy that Powersliding offers: Fresh Fruit.";

            wrapText(
                desc,
                rightBoundary,
                (Ogre::Font*)Ogre::FontManager::getSingleton().getByName(label->getTextArea()->getFontName()).getPointer(),
                label->getTextArea()->getSpaceWidth(),
                label->getTextArea()->getCharHeight());

            label->getTextArea()->setCaption(desc);
        }
    }
}