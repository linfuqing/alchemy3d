package cn.alchemy3d.scene
{
	import cn.alchemy3d.polygon.Polygon;
	
	public class Scene3D
	{
		public var childrenNum:int = 0;
		public var children:Vector.<Polygon> = new Vector.<Polygon>();
		
		public function Scene3D()
		{
		}

		public function addChild(child:Polygon):void
		{
			if(child.parent) throw new Error("已经存在父级.");
			
			child.parent = null;
			child.root = child;
			child.scene = this;
			
			children.push(child);
			childrenNum ++;
		}
	}
}