package cn.alchemy3d.scene
{

	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.lights.Light3D;
	import cn.alchemy3d.objects.Entity;
	import cn.alchemy3d.objects.Mesh3D;
	
	import flash.utils.Dictionary;
	
	public class Scene3D implements IDevice
	{
		public function Scene3D()
		{
			children = new Dictionary();
			lights = new Vector.<Light3D>();
			
			lib = Alchemy3DLib.getInstance();
		}
		
		public var pointer:uint = 0;
		
		public var children:Dictionary;
		public var lights:Vector.<Light3D>;
		public var childrenNum:int = 0;
		public var facesNum:int = 0;
		public var verticesNum:int = 0;
		
		protected var lib:Alchemy3DLib;
		
		public function initialize(devicePointer:uint):void
		{	
			this.pointer = lib.alchemy3DLib.initializeScene(devicePointer);
		}
		
		public function allotPtr(ps:Array):void
		{
			
		}
		
		public function addEntity(child:Entity, parent:Entity = null):void
		{
			if(child.parent) throw new Error("已存在父节点");
			
			var parentPtr:uint = 0;
			
			if (parent)
			{
				child.parent = parent;
				child.root = parent.root;
				child.scene = this;
				parentPtr = parent.pointer;
			}
			else
			{
				child.root = child;
				child.scene = this;
			}
			
			children[child.name] = child;
			childrenNum ++;
			
			//创建实体并添加到指定节点
			//返回该对象起始指针
			child.initialize(this.pointer, parentPtr);
			
			if (child is Mesh3D)
			{
				verticesNum += Mesh3D(child).vertices.length;
				facesNum += Mesh3D(child).faces.length;
			}
			/*var arr:Array;
			if (child is Mesh3D)
			{
				var mesh:Mesh3D = Mesh3D(child);
				
				verticesNum += mesh.vertices.length;
				facesNum += mesh.faces.length;
				
				//把顶点和面信息压入内存
				mesh.fillVerticesToBuffer();
				mesh.fillFacesToBuffer();
//				lib.buffer.position = mesh.tmpBuffPointer;
//				for (var i:int = 0; i < mesh.vertices.length * 4 * 4 + mesh.faces.length * 9 * 4 + 500; i++)
//				{
//					trace(i + " : " + lib.buffer.readByte());
//				}
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPtr, mesh.tmpBuffPointer, mesh.vertices.length, mesh.faces.length);

				mesh.pointer = arr[0];
				mesh.positionPtr = arr[1];
				mesh.directionPtr = arr[2];
				mesh.scalePtr = arr[3];
				mesh.verticesPointer = arr[4];
				mesh.facesPointer = arr[5];
			}
			else
			{
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPtr, 0, 0, 0, 0);
				child.pointer = arr[0];
				child.positionPtr = arr[1];
				child.directionPtr = arr[2];
				child.scalePtr = arr[3];
			}*/
		}
		
		public function addLight(light:Light3D):void
		{
			light.initialize(this.pointer, 0);
			
			lights.push(light);
		}

		public function getChildByName(name:String):Entity
		{
			return this.children[name];
		}
	}
}