package cn.objects
{
	import __AS3__.vec.Vector;
	
	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.geom.Mesh;
	import cn.alchemy3d.objects.Object3D;
	import cn.alchemy3d.objects.Viewport;
	
	public class Scene3D extends Object3D
	{
		private const MOVE_TYPE_ADDED_SCENE:int = 3;
		
		private var children:Vector.<Scene3D>;
		private var viewports:Vector.<Viewport>;
		
		private var _visible:Boolean;
		private var _mesh   :Mesh;
		
		private var scenePointer    :uint;

		private var meshPointer     :uint;
		private var objectPointer   :uint;
		private var visiblePointer  :uint;
		
		public function set mesh( mesh:Mesh ):void
		{
			_mesh = mesh;
			
			Library.instance().buffer.position = meshPointer;
			
			Library.instance().buffer.writeUnsignedInt( mesh.pointer );
		}
		
		public function get mesh():Mesh
		{
			return _mesh;
		}
		
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
			_mesh = mesh;
			
			children = new Vector.<Scene3D>();
			
			super();
		}
		
		override public function initialize():void
		{
			super.initialize();
			
			var scene:Array = Library.instance().methods.initializeScene( _mesh ? _mesh.pointer : NULL, _pointer );
			
			objectPointer    = _pointer;
			
			_pointer         = scene[0];
			
			scenePointer     = scene[1];

			visiblePointer   = scene[2];
			
			meshPointer      = scene[3];
		}
		
		public function addChild( child:Scene3D ):void
		{
			Library.instance().methods.sceneAddChild( scenePointer, child.scenePointer );
			 
			children.push( child );
		}
		
		public function removeChild( child:Scene3D, all:Boolean = true ):Boolean
		{
			var numChildren:Number = children.length;
			
			for( var i:uint = 0; i < numChildren; i ++ )
			{
				if( children[i] === child )
				{
					Library.instance().methods.sceneAddChild( scenePointer, child.scenePointer );
					
					delete children[i];
					
					return true;
				}
				else if( all && children[i].removeChild( child ) )
				{
					return true;
				}
			}
			
			return false;
		}
		
		public function addViewport( v:Viewport ):void
		{
			Library.instance().methods.screenToWorld( _pointer, v.pointer );
			
			viewports.push( v );
		}
	}
}