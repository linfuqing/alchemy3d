// A OK version

package AlchemyLib.container
{
	import AlchemyLib.animation.MorphAnimation;
	import AlchemyLib.animation.SkeletalAnimation;
	import AlchemyLib.base.Library;
	import AlchemyLib.events.ContainerEvent;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.tools.Debug;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	
	public class Entity extends SceneContainer
	{
		public static const TERRAIN_TRACE_FIXED_HEIGHT:int = 1;
		public static const TERRAIN_TRACE_BOUNDARY    :int = 2;
		
		public var name:String;
		public var id:String;
		public var sid:String;
		
		public function get visible():Boolean
		{
			Library.memory.position = visiblePointer;
			
			return Library.memory.readInt() ? true : false;
		}
		
		public function set  visible(value:Boolean):void
		{
			Library.memory.position = visiblePointer;
			
			Library.memory.writeInt(value ? TRUE : FALSE);
		}

		public function get parent():Entity
		{
			return _parent;
		}
		
		public function get morphAnimation():MorphAnimation
		{
			return _morphAnimation;
		}
		
		public function set morphAnimation(animation:MorphAnimation):void
		{
			Library.memory.position = morphChannelPtr;
			
			Library.memory.writeUnsignedInt(animation.pointer);
			
			_morphAnimation = animation;
		}
		
		public function get skeletalAnimation():SkeletalAnimation
		{
			return _skeletalAnimation;
		}
		
		public function set skeletalAnimation(animation:SkeletalAnimation):void
		{
			Library.memory.position = skeletalChannelPtr;
			
			Library.memory.writeUnsignedInt(animation.pointer);
			
			_skeletalAnimation = animation;
		}
		
		public function set bone(value:Boolean):void
		{
			Library.memory.position = bonePtr;
			Library.memory.writeInt(value ? TRUE : FALSE);
		}
		
		public function get bone():Boolean
		{
			Library.memory.position = bonePtr;
			
			return Library.memory.readInt() ? true : false;
		}
		
		public function set terrainTrace(value:int):void
		{
			Library.memory.position = terrainTracePointer;
			Library.memory.writeInt(value);
		}
		
		public function get mesh():Mesh3D
		{
			return _mesh;
		}
		
		public function set mesh(mesh:Mesh3D):void
		{
			var temp:Mesh3D = _mesh;
			
			if(temp && temp != mesh)
			{
				_mesh = null;
				
				temp.dispatchEvent( new ContainerEvent(ContainerEvent.REMOVED_FROM_ENTITY, this) );
			}
			
			_mesh = mesh;
			
			if(mesh)
			{
				mesh.dispatchEvent( new ContainerEvent(ContainerEvent.ADDED_TO_ENTITY, this) );
			
				Library.memory.position = meshPtr;
				Library.memory.writeUnsignedInt(mesh.pointer);
			}
		}
		
		public function set transform(matrix:Matrix3D):void
		{
			var rawData:Vector.<Number> = matrix.rawData;
			
			Library.memory.position = transformPtr;
			Library.memory.writeFloat(rawData[0]);
			Library.memory.writeFloat(rawData[1]);
			Library.memory.writeFloat(rawData[2]);
			Library.memory.writeFloat(rawData[3]);
			
			Library.memory.writeFloat(rawData[4]);
			Library.memory.writeFloat(rawData[5]);
			Library.memory.writeFloat(rawData[6]);
			Library.memory.writeFloat(rawData[7]);
			
			Library.memory.writeFloat(rawData[8]);
			Library.memory.writeFloat(rawData[9]);
			Library.memory.writeFloat(rawData[10]);
			Library.memory.writeFloat(rawData[11]);
			
			Library.memory.writeFloat(rawData[12]);
			Library.memory.writeFloat(rawData[13]);
			Library.memory.writeFloat(rawData[14]);
			Library.memory.writeFloat(rawData[15]);
			
			Library.memory.position = transformDirtyPtr;
			Library.memory.writeInt(TRUE);
			
			Library.memory.position = matrixDirtyPtr;
			Library.memory.writeInt(FALSE);
		}
		
		public function get transform():Matrix3D
		{
			var rawData:Vector.<Number> = new Vector.<Number>();
			
			Library.memory.position = transformPtr;
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			rawData.push( Library.memory.readFloat() );
			
			return new Matrix3D(rawData);
		}
		
		public function get direction():Vector3D
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = directionPtr;
			_direction.x = Library.memory.readFloat();
			_direction.y = Library.memory.readFloat();
			_direction.z = Library.memory.readFloat();
			
			return _direction;
		}
		
		public function set direction(direction:Vector3D):void
		{
			transformDirty();
			
			Library.memory.position = directionPtr;
			Library.memory.writeFloat(direction.x);
			Library.memory.writeFloat(direction.y);
			Library.memory.writeFloat(direction.z);
		}
		
		public function get position():Vector3D
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = positionPtr;
			_position.x = Library.memory.readFloat();
			_position.y = Library.memory.readFloat();
			_position.z = Library.memory.readFloat();
			
			return _position;
		}
		
		public function set position(position:Vector3D):void
		{
			transformDirty();
			
			Library.memory.position = positionPtr;
			Library.memory.writeFloat(position.x);
			Library.memory.writeFloat(position.y);
			Library.memory.writeFloat(position.z);
		}
		
		public function get worldPosition():Vector3D
		{
			Library.memory.position = worldPositionPtr;
			_worldPosition.x = Library.memory.readFloat();
			_worldPosition.y = Library.memory.readFloat();
			_worldPosition.z = Library.memory.readFloat();
			
			return _worldPosition;
		}
		
		public function get scale():Number
		{
			if( _scale.x == _scale.y && _scale.x == _scale.z )
				return _scale.x;
			else
				return NaN;
		}
		
		public function set scale(value:Number):void
		{
			_scale.x = _scale.y = _scale.z = value;
			Library.memory.position = scalePtr;
			Library.memory.writeFloat(value);
			Library.memory.writeFloat(value);
			Library.memory.writeFloat(value);
			transformDirty();
		}
		
		public function get x():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = positionPtr;
			return Library.memory.readFloat();
		}
		
		public function set x(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = positionPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get y():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = positionPtr + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set y(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = positionPtr + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get z():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = positionPtr + Library.floatTypeSize + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set z(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = positionPtr + Library.floatTypeSize + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleX():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = scalePtr;
			return Library.memory.readFloat();
		}
		
		public function set scaleX(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = scalePtr;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleY():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = scalePtr + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set scaleY(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = scalePtr + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get scaleZ():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = scalePtr + Library.floatTypeSize + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set scaleZ(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = scalePtr + Library.floatTypeSize + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function set rotationX(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = directionPtr;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationX():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = directionPtr;
			return Library.memory.readFloat();
		}
		
		public function set rotationY(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = directionPtr + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationY():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = directionPtr + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set rotationZ(value:Number):void
		{
			transformDirty();
			
			Library.memory.position = directionPtr + Library.floatTypeSize + Library.floatTypeSize;
			Library.memory.writeFloat(value);
		}
		
		public function get rotationZ():Number
		{
			Library.memory.position = matrixDirtyPtr;
			
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = directionPtr + Library.floatTypeSize + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function Entity(name:String = "", mesh:Mesh3D = null)
		{
			this.name = name;
			this.visible = true;
			
			_direction = new Vector3D();
			_position = new Vector3D();
			_scale = new Vector3D();
			_worldPosition = new Vector3D();
			
			addEvent = new ContainerEvent(ContainerEvent.ADDED_TO_ENTITY, this);
			removeEvent = new ContainerEvent(ContainerEvent.REMOVED_FROM_ENTITY, this);
			
			super();
			
			this.mesh = mesh;
		}
		
		static public function serialize(input:Entity, data:ByteArray):void
		{
			if(!input || !data)
			{
				Debug.warning("no input.");
				
				return;
			}
			
			data.writeUTF(input.name ? input.name : "");
			data.writeUTF(input.id   ? input.id   : "");
			data.writeUTF(input.sid  ? input.sid  : "");
			
			data.writeBoolean(input.bone);
			
			Library.memory.position = input.transformPtr;
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			data.writeFloat( Library.memory.readFloat() );
			
			var length:uint;
			data.writeUnsignedInt(length = input.children.length);
			
			for(var i:uint = 0; i < length; i ++)
				serialize(input.children[i], data);
		}
		
		static public function unserialize(data:ByteArray, info:Entity):Entity
		{
			if(!data)
			{
				Debug.warning("no input.");
				
				return null;
			}
			
			var output:Entity = info ? info : new Entity();
			
			output.name       = data.readUTF();
			output.id         = data.readUTF();
			output.sid        = data.readUTF();
			output.bone       = data.readBoolean();
			
			var rawData:Vector.<Number> = new Vector.<Number>();
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			rawData.push( data.readFloat() );
			
			output.transform = new Matrix3D(rawData);
			
			var length:uint = data.readUnsignedInt();
			
			for(var i:uint = 0; i < length; i ++)
				output.addChild( unserialize(data, null) );
			
			return output;
		}
		
		override public function addChild(child:Entity):void
		{
			if(child._parent)
				child._parent.removeChild(child, false);
			
			super.addChild(child);
			
			child._parent = this;
		}
		
		override public function removeChild(child:Entity, all:Boolean):Entity
		{
			child._parent = null;
			
			return super.removeChild(child, all);
		}
		
		public function getRelative(info:String):Entity
		{
			if(_parent)
			{
				var length:uint = _parent.children.length;
				var relative:Entity;
				for(var i:uint = 0; i < length; i ++)
					if( (relative = _parent.children[i]) != this && 
						( relative.name == info || relative.id == info || relative.sid == info ||
						( relative = relative.getChild(info) ) ) )
						return relative;
				
				return _parent.getRelative(info);
			}
			
			return null;
		}
		
		public function getDistantRelative(info:String):Entity
		{
			if(_parent)
				return _parent.getRelative(info);
			
			return null;
		}
		
		public function getWorldAABB(min:Vector3D, max:Vector3D):void
		{
			if(min)
			{
				Library.memory.position = localAABBPointer;
				min.x = Library.memory.readFloat();
				min.y = Library.memory.readFloat();
				min.z = Library.memory.readFloat();
				min.w = Library.memory.readFloat();
			}
			
			if(max)
			{
				Library.memory.position = localAABBPointer + 4 * Library.floatTypeSize;
				max.x = Library.memory.readFloat();
				max.y = Library.memory.readFloat();
				max.z = Library.memory.readFloat();
				max.w = Library.memory.readFloat();
			}
		}
		
		public function getLocalAABB(min:Vector3D, max:Vector3D):void
		{
			if(min)
			{
				Library.memory.position = worldAABBPointer;
				min.x = Library.memory.readFloat();
				min.y = Library.memory.readFloat();
				min.z = Library.memory.readFloat();
				min.w = Library.memory.readFloat();
			}
			
			if(max)
			{
				Library.memory.position = worldAABBPointer + 4 * Library.floatTypeSize;
				max.x = Library.memory.readFloat();
				max.y = Library.memory.readFloat();
				max.z = Library.memory.readFloat();
				max.w = Library.memory.readFloat();
			}
		}
		
		public function setLocalAABB(min:Vector3D, max:Vector3D):void
		{
			if(min)
			{
				Library.memory.position = localAABBPointer;
				Library.memory.position = Library.memory.readUnsignedInt();
				
				Library.memory.writeFloat(min.x);
				Library.memory.writeFloat(min.y);
				Library.memory.writeFloat(min.z);
				Library.memory.writeFloat(min.w);
			}
			
			if(max)
			{
				Library.memory.position = localAABBPointer + Library.intTypeSize;
				Library.memory.position = Library.memory.readUnsignedInt();
				
				Library.memory.writeFloat(max.x);
				Library.memory.writeFloat(max.y);
				Library.memory.writeFloat(max.z);
				Library.memory.writeFloat(max.w);
			}
		}
		
		public function right(steps:Number):void
		{
			var vector:Vector3D = new Vector3D();
			
			Library.memory.position = transformPtr;
			
			vector.x = Library.memory.readFloat();
			vector.y = Library.memory.readFloat();
			vector.z = Library.memory.readFloat();
			vector.w = Library.memory.readFloat();
			
			if(vector.w && vector.w != 1)
				vector.project();
			
			vector.normalize();
			
			vector.scaleBy(steps);
			
			position = position.add(vector);
		}
		
		public function up(steps:Number):void
		{
			var vector:Vector3D = new Vector3D();
			
			Library.memory.position = transformPtr + 4 * Library.floatTypeSize;
			
			vector.x = Library.memory.readFloat();
			vector.y = Library.memory.readFloat();
			vector.z = Library.memory.readFloat();
			vector.w = Library.memory.readFloat();
			
			if(vector.w && vector.w != 1)
				vector.project();
			
			vector.normalize();
			
			vector.scaleBy(steps);
			
			position = position.add(vector);
		}
		
		public function forward(steps:Number):void
		{
			var vector:Vector3D = new Vector3D();
			
			Library.memory.position = transformPtr + 8 * Library.floatTypeSize;
			
			vector.x = Library.memory.readFloat();
			vector.y = Library.memory.readFloat();
			vector.z = Library.memory.readFloat();
			vector.w = Library.memory.readFloat();
			
			if(vector.w && vector.w != 1)
				vector.project();
			
			vector.normalize();
			
			vector.scaleBy(steps);
			
			position = position.add(vector);
		}
		
		protected function transformDirty():void
		{
			Library.memory.position = matrixDirtyPtr;
			if( !Library.memory.readInt() )
				Library.alchemy3DLib.decomposeEntity(_pointer);
			
			Library.memory.position = transformDirtyPtr;
			Library.memory.writeInt(TRUE);
		}
		
		override protected function initialize():void
		{
			var ps:Array = Library.alchemy3DLib.initializeEntity(name, _mesh ? _mesh.pointer : NULL);

			_pointer		      = ps[0];
			positionPtr			  = ps[1];
			directionPtr		  = ps[2];
			scalePtr			  = ps[3];
			worldPositionPtr	  = ps[4];
			meshPtr               = ps[5];
			transformDirtyPtr     = ps[6];
			_node                 = ps[7];
			IDPtr                 = ps[8];
			bonePtr               = ps[9];
			transformPtr          = ps[10];
			matrixDirtyPtr        = ps[11];
			morphChannelPtr       = ps[12];
			skeletalChannelPtr    = ps[13];
			terrainTracePointer   = ps[14];
			localAABBPointer      = ps[15];
			worldAABBPointer      = ps[16];
			visiblePointer        = ps[17];
		}
		
		
		private var directionPtr:uint;
		private var positionPtr:uint;
		private var scalePtr:uint;
		private var worldPositionPtr:uint;
		private var meshPtr:uint;
		private var transformDirtyPtr:uint;
		private var IDPtr:uint;
		private var bonePtr:uint;
		private var transformPtr:uint;
		private var matrixDirtyPtr:uint;
		private var morphChannelPtr:uint;
		private var skeletalChannelPtr:uint;
		private var terrainTracePointer:uint;
		private var localAABBPointer:uint;
		private var worldAABBPointer:uint;
		private var visiblePointer:uint;
		
		private var _direction:Vector3D;
		private var _position:Vector3D;
		private var _scale:Vector3D;
		private var _worldPosition:Vector3D;
		
		private var _skeletalAnimation:SkeletalAnimation;
		private var _morphAnimation:MorphAnimation;
		private var _mesh:Mesh3D;
		private var _parent:Entity;
		
		
		private var removeEvent:ContainerEvent;
		private var addEvent:ContainerEvent;
	}
}