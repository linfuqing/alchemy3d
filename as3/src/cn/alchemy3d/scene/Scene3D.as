package cn.alchemy3d.scene
{

	import cn.alchemy3d.lib.Alchemy3DLib;
	
	public class Scene3D
	{
		public var pointer:int;
		
		public function Scene3D()
		{
			var lib:Alchemy3DLib = Alchemy3DLib.getInstance();
			
			//初始化场景
			pointer = lib.alchemy3DLib.initializeScene();
			
			trace(pointer);
			lib.alchemy3DLib.test(pointer);
		}
	}
}