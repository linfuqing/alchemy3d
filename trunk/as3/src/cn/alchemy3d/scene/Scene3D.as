package cn.alchemy3d.scene
{

	import cn.alchemy3d.cameras.Camera3D;
	import cn.alchemy3d.device.IDevice;
	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.objects.Mesh3D;
	
	import flash.utils.Dictionary;
	
	public class Scene3D implements IDevice
	{
		public function Scene3D()
		{
			children = new Dictionary();
			
			lib = Alchemy3DLib.getInstance();
		}
		
		public var pointer:uint = 0;
		
		public var children:Dictionary;
		public var childrenNum:int = 0;
		public var facesNum:int = 0;
		public var verticesNum:int = 0;
		
		protected var lib:Alchemy3DLib;
		
		public function initialize(devicePointer:uint):void
		{	
			this.pointer = lib.alchemy3DLib.initializeScene(devicePointer);
		}
		
		public function addChild(child:DisplayObject3D, parent:* = null):void
		{
			if(child.parent) throw new Error("以存在父节点");
			if(child is Camera3D) throw new Error("场景中不能添加摄像机");
			
			var parentInstance:DisplayObject3D;
			var parentPointer:uint = 0;
			
			if (parent)
			{
				parentInstance = (parent is String) ? children[parent] : parent;
				
				if (parentInstance)
				{
					child.parent = parentInstance;
					child.root = parentInstance.root;
					child.scene = this;
					
					parentPointer = parentInstance.pointer;
				}
				else
					throw new Error("不存在该节点");
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
			var arr:Array;
			if (child is Mesh3D)
			{
				var mesh:Mesh3D = Mesh3D(child);
				
				verticesNum += mesh.vertices.length;
				facesNum += mesh.faces.length;
				
				//把顶点和面信息压入内存
				mesh.fillVerticesToBuffer();
				mesh.fillFacesToBuffer();
				lib.buffer.position = mesh.tmpBuffPointer;
//				for (var i:int = 0; i < 34; i++)
//				{
//					trace(i + " : " + lib.buffer.readFloat());
//				}
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPointer, mesh.tmpBuffPointer, mesh.vertices.length, mesh.faces.length);

				mesh.pointer = arr[0];
				mesh.positionPtr = arr[1];
				mesh.directionPtr = arr[2];
				mesh.scalePtr = arr[3];
				mesh.verticesPointer = arr[4];
				mesh.facesPointer = arr[5];
			}
			else
			{
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPointer, 0, 0, 0, 0);
				child.pointer = arr[0];
				child.positionPtr = arr[1];
				child.directionPtr = arr[2];
				child.scalePtr = arr[3];
			}
		}

		public function getChildByName(name:String):DisplayObject3D
		{
			return this.children[name];
		}
	}
}