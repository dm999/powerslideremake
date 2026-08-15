#ifndef MODECONTEXT_H
#define MODECONTEXT_H

#include "../GameState.h"
#include "../sound/SoundsProcesser.h"
#include "../sound/MusicProcessor.h"

#include "../gamelogic/LapController.h"

#include <vector>
#include <string>

//Deathmatch post-race statistics: one row per car (player + each AI).
//Carried from DeathmatchMode (destroyed by GameModeSwitcher::clear()) to the
//post-race menu so the statistics screen can render it. mTime is seconds of
//the race clock (player total race time, or an AI's elimination time); an AI
//that was still alive at session end has mSurvived == true and mTime == -1.0f.
struct DeathmatchResultRow
{
    std::string mName;
    bool mIsPlayer;
    Ogre::Real mTime;
    bool mSurvived;

    DeathmatchResultRow() : mIsPlayer(false), mTime(-1.0f), mSurvived(false){}
    DeathmatchResultRow(const std::string& name, bool isPlayer, Ogre::Real time, bool survived)
        : mName(name), mIsPlayer(isPlayer), mTime(time), mSurvived(survived){}

    //ranking for the post-race statistics table: survivors above eliminated,
    //the player (winner) first among survivors, and among eliminated AI the
    //latest-eliminated (largest elimination time) above the earliest-eliminated
    //(smallest time) — so the table reads best-to-worst survival downward.
    //A survivor with a valid finish time (mTime >= 0) ranks above a DNF
    //survivor (mTime < 0, didn't complete all laps) — so a player who DNF'd
    //drops below any AI that actually finished.
    static bool sortByRank(const DeathmatchResultRow& a, const DeathmatchResultRow& b)
    {
        //survivors rank above the eliminated.
        if(a.mSurvived != b.mSurvived) return a.mSurvived;
        if(a.mSurvived)
        {
            //a valid finish time (>= 0) ranks above a DNF (< 0).
            const bool aHasTime = (a.mTime >= 0.0f);
            const bool bHasTime = (b.mTime >= 0.0f);
            if(aHasTime != bHasTime) return aHasTime;
            //both in the same category: the player comes first.
            if(a.mIsPlayer != b.mIsPlayer) return a.mIsPlayer;
            //surviving AI: deterministic order by name.
            return a.mName < b.mName;
        }
        //both eliminated: later elimination (larger time) ranks higher,
        //earlier elimination (smaller time) sinks to the bottom.
        if(a.mTime != b.mTime) return a.mTime > b.mTime;
        return a.mName < b.mName;
    }
};
typedef std::vector<DeathmatchResultRow> deathmatchResultVec;

namespace Ogre
{
    class Root;
    class RenderWindow;
}

namespace MyGUI
{
    class Gui;
    class OgrePlatform;
}

struct lua_State;

class InputHandler;
class CustomTrayManager;
class CustomOverlaySystem;
class GameModeSwitcher;
class BaseApp;

class ModeContext
{
public:

    ModeContext(
        Ogre::Root* root, Ogre::RenderWindow* window,
        InputHandler* inputHandler,
        CustomTrayManager* trayMgr, CustomOverlaySystem* overlaySystem,
        lua_State * pipeline,
        GameState& gameState,
        BaseApp * baseApp
#ifndef NO_OPENAL
        ,SoundsProcesser& soundsProcesser,
        MusicProcessor& musicProcessor
#endif
    );

    ModeContext(const ModeContext& other);

    ~ModeContext(){}

    void setGameModeSwitcher(GameModeSwitcher* gameModeSwitcher){mGameModeSwitcher = gameModeSwitcher;}
    GameModeSwitcher * getGameModeSwitcher() const{return mGameModeSwitcher;}

    GameState& getGameState(){return mGameState;}
    const GameState& getGameState() const{return mGameState;}

    Ogre::RenderWindow* getRenderWindow(){return mWindow;}

    CustomTrayManager* getTrayManager(){return mTrayMgr;}

    BaseApp* getBaseApp(){return mBaseApp;}

    void setLapController(const LapController& lapController){mLapController = lapController;}
    const LapController& getLapController() const{return mLapController;}

    void setFinishBoard(const finishBoardVec& finishBoard){mFinishBoard = finishBoard;}
    const finishBoardVec& getFinishBoard() const{return mFinishBoard;}

    void setDeathmatchResults(const deathmatchResultVec& results){mDeathmatchResults = results;}
    const deathmatchResultVec& getDeathmatchResults() const{return mDeathmatchResults;}

    //massacre mode score/statistics (copied from BaseRaceMode on deathmatch teardown).
    void setDeathmatchEliminatedCount(size_t count){mDeathmatchEliminatedCount = count;}
    size_t getDeathmatchEliminatedCount() const{return mDeathmatchEliminatedCount;}
    void setDeathmatchInjuredCount(size_t count){mDeathmatchInjuredCount = count;}
    size_t getDeathmatchInjuredCount() const{return mDeathmatchInjuredCount;}
    void setDeathmatchScore(size_t score){mDeathmatchScore = score;}
    size_t getDeathmatchScore() const{return mDeathmatchScore;}

#ifndef NO_OPENAL
    SoundsProcesser& getSoundsProcesser(){return mSoundsProcesser;}
    MusicProcessor& getMusicProcessor(){return mMusicProcessor;}
#endif

    friend class GameModeSwitcher;
    friend class BaseMenuMode;
    friend class MenuMode;
    friend class MenuMultiMode;
    friend class UIMainMenu;
    friend class UIMainMenuMulti;
    friend class BaseRaceMode;
    friend class SinglePlayerMode;
    friend class MultiPlayerMode;
    friend class UIBackground;
    friend class UIBackgroundLoaderProgress;
    friend class UIBackgroundLoaderProgressTracks;

private:

    ModeContext& operator=(const ModeContext&);

    Ogre::Root* mRoot;
    Ogre::RenderWindow* mWindow;

    InputHandler* mInputHandler;

    CustomTrayManager* mTrayMgr;
    CustomOverlaySystem* mOverlaySystem;

    lua_State * mPipeline;

    GameModeSwitcher* mGameModeSwitcher;

    GameState& mGameState;

    BaseApp * mBaseApp;

    LapController mLapController;

    finishBoardVec mFinishBoard;

    deathmatchResultVec mDeathmatchResults;

    //massacre stats (copied from BaseRaceMode on deathmatch teardown).
    size_t mDeathmatchEliminatedCount;
    size_t mDeathmatchInjuredCount;
    size_t mDeathmatchScore;

#ifndef NO_OPENAL
    SoundsProcesser& mSoundsProcesser;
    MusicProcessor& mMusicProcessor;
#endif
};

#endif