package cn.alchemy3d.container
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Node;
	import cn.alchemy3d.lights.Light3D;

	public class SceneContainer extends Node
	{
		private var children:Vector.<Entity>;
		private var lights:Vector.<Light3D>;
		
		public function get numChildren():uint
		{
			return children.length;
		}
		
		public function get numLights():uint
		{
			return lights.length;
		}
		
		public function SceneContainer()
		{
			children = new Vector.<Entity>();
			lights   = new Vector.<Light3D>();
			super();
		}
		
		protected function isChild():uint
		{
			return TRUE;
		}
		
		public function addChild( child:Entity ):void
		{
			if( _node )
			{
				Library.alchemy3DLib.addEntity( _node, child.node, isChild() );
			}
			else
			{
				_node = child.node;
			}
			
			Library.alchemy3DLib.addLight( getLightsPointerOn( child ), lightsPtr );
			children.push( child );
		}
		
		public function removeChild( child:Entity, all:Boolean ):Entity
		{
			for( var i:uint = 0; i < children.length; i ++ )
			{
				if( child === children[i] )
				{
					delete children[i];
					
					Library.alchemy3DLib.removeLight( getLightsPointerOn( child ), lightsPtr, FALSE );
					Library.alchemy3DLib.removeEntity( _node, child.node );
					
					return child;
				}
				
				if( all )
				{
					if( children[i].removeChild( child, true ) )
					{
						return child;
					}
				}
			}
							
			return null;
		}
		
		public function addLight( light:Light3D ):void
		{
			if( lightsPtr )
			{
				Library.alchemy3DLib.addLight( lightsPtr, light.node );
			}
			else
			{
				lightsPtr = light.node;
			}
			
			lights.push( light );
		}
		
		public function removeLight( light:Light3D ):Light3D
		{
			for( var i:uint = 0; i < lights.length; i ++ )
			{
				if( lights[i] === light )
				{
					delete lights[i];
					
					Library.alchemy3DLib.removeLight( lightsPtr, light.node, TRUE );
					
					return light;
				}
			}
			
			return null;
		}
		
				
		protected static function getLightsPointerOn( s:SceneContainer ):uint
		{
			return s.lightsPtr;
		}
		
				
		private var lightsPtr:uint = NULL;
	}
}