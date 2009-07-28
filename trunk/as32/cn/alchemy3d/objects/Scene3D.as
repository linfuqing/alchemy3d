package cn.objects
{
	import cn.alchemy3d.base.Instance;
	import cn.geom.Mesh3D;
	
	public class Scene3D extends Instance
	{
		private var cameraPointer :uint;
		private var meshPointer   :uint;
		private var visiblePointer:uint;
		
		public function set visible( value:Boolean ):void
		{
			
		}
		
		public function Scene3D( mesh:Mesh3D )
		{
			super();
			
			instance.library.alchemy3DLib.initializeScene( mesh.pointer );
		}

	}
}