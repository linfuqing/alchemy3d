package AlchemyLib.container
{
	import __AS3__.vec.Vector;
	
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Node;
	import AlchemyLib.fog.Fog;
	import AlchemyLib.lights.Light3D;

	public class SceneContainer extends Node
	{
		protected var children:Vector.<Entity>;
		protected var lights:Vector.<Light3D>;
		
		private var fog:Fog;
		
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
		
		public override function destroy(all:Boolean):void
		{
			var length:uint = children.length;
			
			for(var i:uint = 0; i < length; i ++)
				children[i].destroy(all);
			
			children = null;
			lights = null;
		}
		
		protected function isChild():uint
		{
			return TRUE;
		}
		
		public function getChildByIndex(index:uint):Entity
		{
			return children[index];
		}
		
		public function getChild(info:String):Entity
		{
			var child:Entity;
			for(var i:uint = 0; i < children.length; i ++)
			{
				child = children[i];
				
				if(child.name == info || child.id == info || child.sid == info || ( child = child.getChild(info) ) != null)
					return child;
			}
			
			return null;
		}
		
		public function getChildByName(name:String):Entity
		{
			var child:Entity;
			for(var i:uint = 0; i < children.length; i ++)
			{
				child = children[i];
				
				if(child.name == name || ( child = child.getChildByName(name) ) != null)
					return child;
			}
			
			return null;
		}
		
		public function contains(child:Entity):Boolean
		{
			var length:uint = children.length;
			for(var i:uint = 0; i < length; i ++)
				if(children[i] === child || children[i].contains(child) )
					return true;
			
			return false;
		}
		
		public function addChild( child:Entity ):void
		{
			if( _node )
				Library.alchemy3DLib.addEntity( _node, child.node, isChild() );
			else
				_node = child.node;
			
			Library.alchemy3DLib.addLight( getLightsPointerOn( child ), lightsPtr );
			children.push( child );
		}
		
		public function removeChild( child:Entity, all:Boolean ):Entity
		{
			for( var i:uint = 0; i < children.length; i ++ )
			{
				if( child === children[i] )
				{
					children.splice(i, 1);
					
					Library.alchemy3DLib.removeLight( getLightsPointerOn( child ), lightsPtr, FALSE );
					Library.alchemy3DLib.removeEntity( _node, child.node );
					
					return child;
				}
				
				if( all && children[i].removeChild( child, true ) )
					return child;
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
		
				
		protected var lightsPtr:uint = NULL;
		
		public function addFog( fog:Fog ):void
		{
			if( ! this.fog && fog )
			{
				Library.alchemy3DLib.addFog( _pointer, fog.pointer );
			}
			else
			{
				return;
			}
			
			this.fog = fog;
		}
		
		public function removeFog( fog:Fog ):Fog
		{
			if( this.fog && fog )
			{
				Library.alchemy3DLib.removeFog( _pointer, fog.pointer );
					
				return fog;
			}
			
			return null;
		}
	}
}