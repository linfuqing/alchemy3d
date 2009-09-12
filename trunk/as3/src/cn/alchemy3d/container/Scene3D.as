package cn.alchemy3d.container
{

	import cn.alchemy3d.base.Library;
	import cn.alchemy3d.base.Pointer;
	import cn.alchemy3d.events.SceneEvent;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.utils.Dictionary;
	
	public class Scene3D extends Pointer
	{
		public function Scene3D()
		{
			nodes = new Dictionary();
			lights = new Vector.<Light3D>();
			
			super();
		}
		
		//public var pointer:uint = 0;
		
		private var nodes:Dictionary;
		private var lights:Vector.<Light3D>;
		private var childrenNum:int = 0;
		
		override protected function initialize():void
		{	
			_pointer = Library.alchemy3DLib.initializeScene();
		}
		
		public function addEntity(node:Entity, parent:Entity = null):void
		{
			if (node.scene || node.parent) Alchemy3DLog.error("已存在父节点");
			
			var parentPtr:uint = 0;
			
			if (parent)
			{
				parentPtr = parent.pointer;
				
				node.parent = parent;
			}
			
			nodes[node.name] = node;
			childrenNum ++;
			
			//创建实体并添加到指定节点
			//返回该对象起始指针
			Library.alchemy3DLib.addEntity(this.pointer, node.pointer, parentPtr);
		}
		
		public function addLight(light:Light3D):void
		{
			light.dispatchEvent( new SceneEvent( SceneEvent.ADDED_TO_SCENE, this ) );
			
			lights.push(light);
		}

		public function getChildByName(name:String):Entity
		{
			return this.nodes[name];
		}
	}
}