package cn.alchemy3d.container
{
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.lights.Light3D;
	
	public class Scene3D extends SceneContainer
	{
		public function Scene3D()
		{
			super();
		}
		
		override protected function initialize():void
		{
			_pointer = Library.alchemy3DLib.initializeScene();
		}
		
		override protected function isChild():uint
		{
			return FALSE;
		}
		
		override public function addChild( child:Entity ):void
		{
			super.addChild( child );

			if( numChildren == 1 )
			{
				Library.alchemy3DLib.attachScene( _pointer, _node, getLightsPointerOn( this ) );
			}
		}
		
		override public function addLight( light:Light3D ):void
		{
			super.addLight(light);
			
			if( numLights == 1 )
			{
				Library.alchemy3DLib.attachScene( _pointer, _node, getLightsPointerOn( this ) );
			}
		}
	}
}