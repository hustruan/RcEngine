#include "FbxImporter.h"
#include "ExportLog.h"

ConsoleOutListener g_ConsoleOutListener;
DebugSpewListener  g_DebugSpewListener;
ExportSettings     g_ExportSettings;

int main()
{
	ExportLog::AddListener( &g_ConsoleOutListener );
#if _MSC_VER >= 1500
	if( IsDebuggerPresent() )
		ExportLog::AddListener( &g_DebugSpewListener );
#endif

	ExportLog::SetLogLevel( 1 );
	ExportLog::EnableLogging( TRUE );

	//g_ExportSettings.ExportSkeleton = true;
	g_ExportSettings.MergeWithSameMaterial = true;
	g_ExportSettings.MergeScene = true;
	g_ExportSettings.SwapWindOrder = true;

	FbxProcesser fbxProcesser;

	//fbxProcesser.LoadOgre("E:/GitHub/RcEngine/RcEngine/Tools/FbxImporter/Sinbad/Sword.mesh");
	//fbxProcesser.BuildAndSaveBinary();
	////fbxProcesser.BuildAndSaveOBJ();
	//fbxProcesser.BuildAndSaveMaterial();


	fbxProcesser.Initialize();

	if (fbxProcesser.LoadScene("../../Media/Mesh/ShuangMaCheng/Castle.FBX"))
	{
		fbxProcesser.ProcessScene();
		fbxProcesser.BuildAndSaveBinary();
		fbxProcesser.BuildAndSaveMaterial();
		fbxProcesser.ExportMaterial();
	}

	return 0;
}

