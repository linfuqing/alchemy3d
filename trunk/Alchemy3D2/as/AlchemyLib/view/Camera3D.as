package AlchemyLib.view
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.container.Entity;
	
	import flash.geom.Vector3D;
	
	public class Camera3D extends Pointer
	{
		public static const HOVER_TYPE_NONE:int     = 0;
		public static const HOVER_TYPE_ABSOLUT:int  = 1;
		public static const HOVER_TYPE_RELATIVE:int = 2;
		
		public function get x():Number
		{
			return _eye.x;
		}
		
		public function set x(value:Number):void
		{
			_eye.x = value;
		}
		
		public function get y():Number
		{
			return _eye.y;
		}
		
		public function set y(value:Number):void
		{
			_eye.y = value;
		}
		
		public function get z():Number
		{
			return _eye.z;
		}
		
		public function set z(value:Number):void
		{
			_eye.z = value;
		}
		
		public function get rotationX():Number
		{
			if(hoverType)
				return targetRotationX;
			
			return _eye.rotationX;
		}
		
		public function set rotationX(value:Number):void
		{
			var hoverType:int = this.hoverType;
			
			if(hoverType)
			{
				Library.memory.position = rotateXPtr;
				//Library.memory.writeFloat(value - targetRotationX);
				Library.memory.writeFloat( hoverType == HOVER_TYPE_ABSOLUT ? value : (value - targetRotationX) );
				
				Library.memory.position = rotateDirtyPtr;
				Library.memory.writeInt(TRUE);
				
				targetRotationX = value;
			}
			else
				_eye.rotationX = value;
		}
		
		public function get rotationY():Number
		{
			if(hoverType)
				return targetRotationY;
			
			return _eye.rotationY;
		}
		
		public function set rotationY(value:Number):void
		{
			var hoverType:int = this.hoverType;
			
			if(hoverType)
			{
				Library.memory.position = rotateYPtr;
				//Library.memory.writeFloat(value - targetRotationY);
				Library.memory.writeFloat( hoverType == HOVER_TYPE_ABSOLUT ? value : (value - targetRotationY) );
				
				Library.memory.position = rotateDirtyPtr;
				Library.memory.writeInt(TRUE);
				
				targetRotationY = value;
			}
			else
				_eye.rotationY = value;
		}
		
		public function get rotationZ():Number
		{
			if(hoverType)
				return targetRotationZ;
			
			return _eye.rotationZ;
		}
		
		public function set rotationZ(value:Number):void
		{
			var hoverType:int = this.hoverType;
			
			if(hoverType)
			{
				Library.memory.position = rotateZPtr;
				//Library.memory.writeFloat(value - targetRotationZ);
				Library.memory.writeFloat( hoverType == HOVER_TYPE_ABSOLUT ? value : (value - targetRotationZ) );
				
				Library.memory.position = rotateDirtyPtr;
				Library.memory.writeInt(TRUE);
				
				targetRotationZ = value;
			}
			else
				_eye.rotationZ = value;
		}
		
		public function get eye():Entity
		{
			return _eye;
		}
		
		public function set hoverType(value:int):void
		{
			Library.memory.position = hoverTypePtr;
			Library.memory.writeInt(value);
		}
		
		public function get hoverType():int
		{
			Library.memory.position = hoverTypePtr;
			return Library.memory.readInt();
		}
		
		public function get target():Vector3D
		{
			if(_target)
			{
				Library.memory.position = targetPtr;
				_target.x = Library.memory.readFloat();
				_target.y = Library.memory.readFloat();
				_target.z = Library.memory.readFloat();
				
				return _target;
			}
			
			return null;
		}
		
		public function set target(target:Vector3D):void
		{
			if (target)
			{
				Library.memory.position = targetPtr;
				Library.memory.writeFloat(target.x);
				Library.memory.writeFloat(target.y);
				Library.memory.writeFloat(target.z);
				
				/*Library.memory.position = isUVNPtr;
				Library.memory.writeInt(TRUE);*/
				
				if(target != _target)
				{
					targetRotationX = 0;
					targetRotationY = 0;
					targetRotationZ = 0;
				}
				
				Library.memory.position = rotateDirtyPtr;
				Library.memory.writeInt(TRUE);
			}
			/*else
			{
				Library.memory.position = isUVNPtr;
				Library.memory.writeInt(FALSE);
			}*/
			
			_target = target;
		}
		
		public function set isUVN(value:Boolean):void
		{
			Library.memory.position = isUVNPtr;
			Library.memory.writeInt(value ? TRUE : FALSE);
		}
		
		public function get isUVN():Boolean
		{
			Library.memory.position = isUVNPtr;
			return Library.memory.readInt() ? true : false;
		}
		
		public function get distance():Number
		{
			Library.memory.position = distancePtr;
			return Library.memory.readFloat();
		}
		
		public function set distance(value:Number):void
		{
			Library.memory.position = distancePtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = rotateDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function get step():Number
		{
			Library.memory.position = stepPtr;
			return Library.memory.readFloat();
		}
		
		public function set step(value:Number):void
		{
			Library.memory.position = stepPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get fov():Number
		{
			return _fov;
		}
		
		public function set fov(value:Number):void
		{
			this._fov = value;
			
			Library.memory.position = fovPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function get far():Number
		{
			return _far;
		}
		
		public function set far(value:Number):void
		{
			this._far = value;
			
			Library.memory.position = farPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function get near():Number
		{
			return _near;
		}
		
		public function set near(value:Number):void
		{
			this._near = value;
			
			Library.memory.position = nearPtr;
			Library.memory.writeFloat(value);
			
			Library.memory.position = fnfDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		public function Camera3D(fov:Number = 90, near:Number = 20, far:Number = 5000, _eye:Entity = null)
		{
			this._fov = fov;
			this._near = near < 20 ? 20 : near;
			this._far = far;
			
			this._eye = _eye == null ? new Entity("camera__eye") : _eye;
			
			super();
		}
		
		public override function destroy(all:Boolean):void
		{
			if(_eye)
			{
				_eye.destroy(true);
				
				_eye = null;
			}
			
			Library.alchemy3DLib.destroyCamera(_pointer);
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeCamera(_eye.pointer, _fov, _near, _far);

			_pointer = ps[0];
			targetPtr = ps[1];
			fovPtr = ps[2];
			nearPtr = ps[3];
			farPtr = ps[4];
			fnfDirtyPtr = ps[5];
			isUVNPtr = ps[6];
			rotateDirtyPtr = ps[7];
			rotateXPtr = ps[8];
			rotateYPtr = ps[9];
			rotateZPtr = ps[10];
			distancePtr = ps[11];
			factorPtr = ps[12];
			stepPtr = ps[13];
			hoverTypePtr = ps[14];
		}
		
		public function hover(mouseX:Number, mouseY:Number, camSpeed:Number):void
		{
			this._eye.x -= (this._eye.x - 1000 * mouseX) / camSpeed;
			this._eye.y -= (this._eye.y - 1000 * mouseY) / camSpeed;
		}
		
		private var targetPtr:uint;
		private var isUVNPtr:uint;
		private var fovPtr:uint;
		private var nearPtr:uint;
		private var farPtr:uint;
		private var fnfDirtyPtr:uint;
		private var rotateDirtyPtr:uint;
		private var rotateXPtr:uint;
		private var rotateYPtr:uint;
		private var rotateZPtr:uint;
		private var distancePtr:uint;
		private var factorPtr:uint;
		private var stepPtr:uint;
		private var hoverTypePtr:uint;
		
		private var _target:Vector3D;
		private var _fov:Number;
		private var _near:Number;
		private var _far:Number;
		
		private var targetRotationX:Number;
		private var targetRotationY:Number;
		private var targetRotationZ:Number;
		
		protected var _eye:Entity;
	}
}