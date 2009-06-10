package cn.alchemy3d.scene
{

	import cn.alchemy3d.polygon.DisplayObject3D;
	
	public class Scene3D
	{
		public function Scene3D()
		{
			_children = new Vector.<DisplayObject3D>();
			index = _childrenNum = 0;
		}
		
		private var _children:Vector.<DisplayObject3D>;
		private var _childrenNum:int;
		private var index:int;
		
		public function get children():Vector.<DisplayObject3D>
		{
			return _children;
		}
		
		public function get childrenNum():int
		{
			return _childrenNum;
		}

		public function addChild(child:DisplayObject3D):void
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
		
		public function getChildAt(index:int):DisplayObject3D
		{
			if (index < _childrenNum)
				return this._children[index];
			else
				return null;
		}
		
		public function getChildByName(name:String):DisplayObject3D
		{
			for(var i:int = 0; i < _childrenNum; i ++)
			{
				if (name == _children[i].name)
					return _children[i];
			}
			return null;
		}
		
		public function removeChild(child:DisplayObject3D):DisplayObject3D
		{
			var i:int = _children.indexOf(child);
			if(i != -1)
			{
				_childrenNum --;
				return _children.splice(i, 1)[0];
			}
			else
				return null;
		}
		
		public function removeChildAt(index:int):DisplayObject3D
		{
			if (index < _childrenNum)
			{
				_childrenNum --;
				return _children.splice(index, 1)[0];
			}
			else
				return null;
		}
		
		public function removeChildByName(name:String):DisplayObject3D
		{
			for(var i:int = 0; i < _childrenNum; i ++)
			{
				if (name == _children[i].name)
				{
					_childrenNum --;
					return removeChildAt(i);
				}
			}
			return null;
		}
	}
}