package cn.alchemy3d.scene
{

	import cn.alchemy3d.objects.DisplayObject3D;
	import cn.alchemy3d.objects.Mesh3D;
	
	public class Scene3D
	{
		public function Scene3D()
		{
			_children = new Vector.<DisplayObject3D>();
			index = _childrenNum = 0;
		}
		
		public var verticesNum:int = 0;
		public var facesNum:int = 0;
		
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
			
			if (child is Mesh3D)
			{
				verticesNum += Mesh3D(child).vertices.length;
				facesNum += Mesh3D(child).faces.length;
			}
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
				if (child is Mesh3D)
				{
					verticesNum -= Mesh3D(child).vertices.length;
					facesNum -= Mesh3D(child).faces.length;
				}
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