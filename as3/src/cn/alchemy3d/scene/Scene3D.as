package cn.alchemy3d.scene
{

	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.Mesh3D;
	import cn.alchemy3d.tools.Alchemy3DLog;
	
	import flash.utils.Dictionary;
	
	public class Scene3D
	{
		public function Scene3D()
		{
			nodes = new Dictionary();
			lights = new Vector.<Light3D>();
		}
		
		public var pointer:uint = 0;
		
		public var nodes:Dictionary;
		public var lights:Vector.<Light3D>;
		public var childrenNum:int = 0;
		public var facesNum:int = 0;
		public var verticesNum:int = 0;
		
		public function initialize(devicePointer:uint):void
		{	
			this.pointer = Library.alchemy3DLib.initializeScene(devicePointer);
		}
		
		public function allotPtr(ps:Array):void
		{
			
		}
		
		public function addEntity(node:Entity, parent:Entity = null):void
		{
			if (node.scene || node.parent) Alchemy3DLog.error("已存在父节点");
			if (!node.pointer || node.pointer == 0)  Alchemy3DLog.error("结点没有实例化");
			
			var parentPtr:uint = 0;
			
			if (parent)
			{
				if (!parent.pointer || parent.pointer == 0)  Alchemy3DLog.error("父结点没有实例化");
				
				parentPtr = parent.pointer;
				
				node.parent = parent;
			}
			
			nodes[node.name] = node;
			childrenNum ++;
			
			//创建实体并添加到指定节点
			//返回该对象起始指针
			Library.alchemy3DLib.addEntity(this.pointer, node.pointer, parentPtr);
			
			if (node is Mesh3D)
			{
				verticesNum += Mesh3D(node).nVertices;
				facesNum += Mesh3D(node).nFaces;
			}
		}
		
		public function addLight(light:Light3D):void
		{
			light.initialize(this);
			
			lights.push(light);
		}

		public function getChildByName(name:String):Entity
		{
			return this.nodes[name];
		}
	}
}