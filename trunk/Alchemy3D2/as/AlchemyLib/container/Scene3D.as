package AlchemyLib.container
{
	import AlchemyLib.base.Library;
	import AlchemyLib.lights.Light3D;
	import AlchemyLib.terrain.MeshTerrain;
	
	public class Scene3D extends SceneContainer
	{
		public function set terrain(t:MeshTerrain):void
		{
			_terrain = t;
			addChild(t);
			
			Library.memory.position = terrainPointer;
			Library.memory.writeUnsignedInt(t.terrainPointer);
		}
		
		public function get terrain():MeshTerrain
		{
			return _terrain;
		}
		
		public function get nNodes():int
		{
			Library.memory.position = nNodesPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get nVertices():int
		{
			Library.memory.position = nVerticesPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get nFaces():int
		{
			Library.memory.position = nFacesPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get nRenderList():int
		{
			Library.memory.position = nRenderListPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get nCullList():int
		{
			Library.memory.position = nCullListPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get nClippList():int
		{
			Library.memory.position = nClippListPointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function Scene3D()
		{
			super();
			
			_terrain = null;
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeScene();
			
			_pointer 			= ps[0];
			nNodesPointer		= ps[1];
			nVerticesPointer	= ps[2];
			nFacesPointer		= ps[3];
			nRenderListPointer	= ps[4];
			nCullListPointer	= ps[5];
			nClippListPointer	= ps[6];
			terrainPointer      = ps[7];
		}
		
		override protected function isChild():uint
		{
			return FALSE;
		}
		
		override public function addChild( child:Entity ):void
		{
			if(child is MeshTerrain)
			{
				_terrain = child as MeshTerrain;
				Library.memory.position = terrainPointer;
				Library.memory.writeUnsignedInt(_terrain.terrainPointer);
			}
			
			super.addChild( child );

			if( numChildren == 1 )
			{
				Library.alchemy3DLib.attachScene( _pointer, _node, lightsPtr );
			}
		}
			
		override public function removeChild(child:Entity, all:Boolean):Entity
		{
			var entity:Entity = super.removeChild(child, all);

			if( child.node == _node )
			{
				_node = children.length ? children[0].node : NULL;
				
				Library.alchemy3DLib.attachScene( _pointer, _node, lightsPtr );
			}
			
			return entity;
		}
		
		override public function addLight( light:Light3D ):void
		{
			super.addLight(light);
			
			if( numLights == 1 )
			{
				Library.alchemy3DLib.attachScene( _pointer, _node, lightsPtr );
			}
		}
		
		override public function removeLight(light:Light3D):Light3D
		{
			var light:Light3D = super.removeLight( light );
			
			if( light.node == lightsPtr )
			{
				lightsPtr = lights.length ? lights[0].node : NULL;
				
				Library.alchemy3DLib.attachScene( _pointer, _node, lightsPtr );
			}
			
			return light;
		}
		
		private var nNodesPointer:uint;
		private var nVerticesPointer:uint;
		private var nFacesPointer:uint;
		private var nRenderListPointer:uint;
		private var nCullListPointer:uint;
		private var nClippListPointer:uint;
		private var terrainPointer:uint;
		
		private var _terrain:MeshTerrain;
	}
}