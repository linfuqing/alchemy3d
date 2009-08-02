package cn.alchemy3d.scene
{

	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Library;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.Mesh3D;
	
	import flash.utils.Dictionary;
	
	public class Scene3D implements IDevice
	{
		public function Scene3D()
		{
			nodes = new Dictionary();
			lights = new Vector.<Light3D>();
			
			lib = Library.getInstance();
		}
		
		public var pointer:uint = 0;
		
		public var nodes:Dictionary;
		public var lights:Vector.<Light3D>;
		public var childrenNum:int = 0;
		public var facesNum:int = 0;
		public var verticesNum:int = 0;
		
		protected var lib:Library;
		
		public function initialize(devicePointer:uint):void
		{	
			this.pointer = lib.alchemy3DLib.initializeScene(devicePointer);
		}
		
		public function allotPtr(ps:Array):void
		{
			
		}
		
		public function addEntity(node:Entity, parent:Entity = null):void
		{
			if (node.scene) throw new Error("已存在父节点");
			
			nodes[node.name] = node;
			childrenNum ++;
			
			//创建实体并添加到指定节点
			//返回该对象起始指针
			node.initialize(this);
			
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