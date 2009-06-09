package cn.alchemy3d.scene
{

	import cn.alchemy3d.polygon.Polygon;
	
	public class Scene3D
	{
		public function Scene3D()
		{
			_children = new Vector.<Polygon>();
			index = _childrenNum = 0;
		}
		
		private var _children:Vector.<Polygon>;
		private var _childrenNum:int;
		private var index:int;
		
		public function get children():Vector.<Polygon>
		{
			return _children;
		}
		
		public function get childrenNum():int
		{
			return _childrenNum;
		}

		public function addChild(child:Polygon):void
		{
			if(child.parent) throw new Error("已经存在父级.");
			
			child.ID = index;
			child.parent = null;
			child.root = child;
			child.scene = this;
			
			_children.push(child);
			_childrenNum ++;
			index ++;
		}
		
		public function getChildByID(id:int):Polygon
		{
			if (id < _childrenNum)
				return this._children[id];
			else
				return null;
		}
		
		public function getChildByName(name:String):Polygon
		{
			for(var i:int = 0; i < _childrenNum; i ++)
			{
				if (name == _children[i].name)
					return _children[i];
			}
			return null;
		}
		
		public function getChildID(Child:Polygon):int
		{
			return _children.indexOf(Child);
		}
		
		public function removeChild(child:Polygon):Polygon
		{
			var i:int = getChildID(child);
			if(i != -1)
			{
				_childrenNum --;
				return _children.splice(i, 1)[0];
			}
			else
				return null;
		}
		
		public function removeChildAt(index:int):Polygon
		{
			if (index < _childrenNum)
			{
				_childrenNum --;
				return _children.splice(index, 1)[0];
			}
			else
				return null;
		}

	}
}