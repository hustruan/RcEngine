#ifndef Application_h__
#define Application_h__

#include <Core/Prerequisites.h>
#include <Core/Timer.h>
#include <MainApp/AppSettings.h>

namespace RcEngine {

class Window;

class _ApiExport Application
{
public:
	Application(const String& config);
	virtual ~Application(void);

	/**
	 * This method will create reader device with render settings and then 
	 * load all modules, create render window
	 */
	void Create(); 

	/**
	 * This method will release all modules.
	 */
	void Release();

	/// <summary>
	// Call this method to initialize the game, begin running the game loop
	// and start processing events for the game.
	/// </summary>
	void RunGame();

	inline Window* GetMainWindow() { return mMainWindow; }
	inline const ApplicationSettings& GetAppSettings() const { return mAppSettings; }

	bool Active() const { return mActice; }
	
protected:

	//Called after the Game and GraphicsDevice are created, but before LoadContent.
	virtual void Initialize() = 0;

	/// <summary>
	// Load Graphical Resources
	// Called when graphics resources need to be loaded.
	/// </summary>
	virtual void LoadContent() = 0;

	/// <summary>
	// UnLoad Graphical Resources
	// Called when graphics resources need to be unloaded. 
	// Override this method to unload any game-specific graphics resources.
	/// </summary>
	virtual void UnloadContent() = 0;

	/// <summary>
	// Called when a frame is ready to be draw
	/// </summary>
	/// <param name ="timer"> The time passed since the last update </param>
	virtual void Render() = 0;

	/// <summary>
	/// Allows the game to perform logic processing
	/// </summary>
	/// <param name ="timer"> The time passed since the last update </param>
	virtual void Update(float deltaTime) = 0;

private:
	void Tick();
	void LoadAllModules();
	void UnloadAllModules();
	void LoadConfiguration();
	void ProcessEventQueue();

	void Window_UserResized();
	void Window_ApplicationActivated();
	void Window_ApplicationDeactivated();
	void Window_Suspend();
	void Window_Resume();
	void Window_Paint();
	void Window_Close();
	
protected:
	virtual void WindowResize(uint32_t width, uint32_t height) { }

protected:

	Window* mMainWindow;

	bool mActice;
	bool mAppPaused;
	bool mMinimized;
	bool mMaximized;
	bool mResizing;

	Timer mTimer;

	ApplicationSettings mAppSettings;
	String mConfigFile; 

	// in case multiple threads are used
	volatile bool mEndGame;	

public:
	static Application* msApp;
};

} // Namespace RcEngine

#endif // Application_h__