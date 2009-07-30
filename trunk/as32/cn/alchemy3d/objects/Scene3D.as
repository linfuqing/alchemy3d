package cn.objects
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	
	public class Scene3D extends Pointer
	{
		private var _visible:Boolean;
		
		private var cameraPointer :uint;
		private var visiblePointer:uint;
		
		public function set visible( value:Boolean ):void
		{
			_visible = value;
			
			if( _pointer )
			{
				Library.instance().buffer.position = visiblePointer;
				
				Library.instance().buffer.writeInt( value ? TRUE : FALSE );
			}
		}
		
		public function get visible():Boolean
		{
			return _visible;
		}
		
		public function Scene3D( mesh:Mesh3D )
		{
			super();
			
			var scene:Array = Library.instance().methods.initializeScene( mesh ? mesh.pointer : NULL );
			
			_pointer       = scene[0];
			
			visiblePointer = scene[1];
			
			cameraPointer  = scene[2];
		}

	}
}