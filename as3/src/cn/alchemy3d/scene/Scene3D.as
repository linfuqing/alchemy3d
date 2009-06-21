package cn.alchemy3d.scene
{

	import cn.alchemy3d.lib.Alchemy3DLib;
	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.objects.Mesh3D;
	
	import flash.utils.Dictionary;
	
	public class Scene3D
	{
		public function Scene3D()
		{
			children = new Dictionary();
			
			lib = Alchemy3DLib.getInstance();
		}
		
		public var pointer:uint;
		
		public var children:Dictionary;
		public var childrenNum:int = 0;
		public var facesNum:int = 0;
		public var verticesNum:int = 0;
		
		protected var lib:Alchemy3DLib;
		
		public function addChild(child:DisplayObject3D, parent:* = null):void
		{
			if(child.parent) throw new Error("以存在父节点");
			
			var parentInstance:DisplayObject3D;
			var parentPointer:uint;
			
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
				
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPointer, mesh.vertices.length * 4, mesh.fillVerticesToBuffer(), mesh.faces.length * 9, mesh.fillFacesToBuffer());
				mesh.pointer = arr[0];
				mesh.positionPtr = arr[1];
				mesh.directionPtr = arr[2];
				mesh.scalePtr = arr[3];
				mesh.verticesPointer = arr[4];
				mesh.facesPointer = arr[5];
			}
			else
			{
				arr = lib.alchemy3DLib.createEntity(this.pointer, parentPointer, 0, null, 0, null);
				child.pointer = arr[0];
				child.positionPtr = arr[1];
				child.directionPtr = arr[2];
				child.scalePtr = arr[3];
			}
		}

	}
}