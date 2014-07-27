#include "AssimpProcesser.h"
#include <unordered_map>

int main(int argc, char** argv)
{
	AssimpProcesser processer;
	//processer.Process("media/Dwarves/dwarf-lod0_rotating_hand.X");
	//processer.Process("media/ninja.mesh.xml");

	vector<String> clips;
	//clips.push_back("dudeWalk.anim");
	//processer.Process("media/teapot.3DS", "media/dude.skeleton", clips);

	processer.Process("E:/Engines/RcEngine/Media/Mesh/Ahri/Ahri.FBX", "media/dude.skeleton", clips);

	return 0;
}