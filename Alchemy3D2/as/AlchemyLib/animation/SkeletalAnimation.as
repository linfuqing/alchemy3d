package AlchemyLib.animation
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.container.Entity;
	import AlchemyLib.tools.Debug;
	
	import flash.geom.Matrix3D;
	import flash.geom.Point;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;

	public class SkeletalAnimation extends Pointer
	{
		public static const VISIBILITY   :int = 0;
		
		public static const TRANSFORM    :int = 1;
		
		public static const TRANSLATION  :int = 2;
		public static const ROTATION     :int = 3;
		public static const SCALE        :int = 4;
		
		public static const TRANSLATION_X:int = 5;
		public static const TRANSLATION_Y:int = 6;
		public static const TRANSLATION_Z:int = 7;
		
		public static const ROTATION_X   :int = 8;
		public static const ROTATION_Y   :int = 9;
		public static const ROTATION_Z   :int = 10;
		
		public static const SCALE_X      :int = 11;
		public static const SCALE_Y      :int = 12;
		public static const SCALE_Z      :int = 13;
		
		public function get name():String
		{
			return _name;
		}
		
		public function SkeletalAnimation(
			name:String, 
			type:int, 
			animation:AnimationObject,
			times:Vector.<Number>, 
			params:Vector.<Vector.<Number>>, 
			matrices:Vector.<Matrix3D>, 
			interpolations:Vector.<Number>,
			inTangent:Vector.<Vector.<Point>>,
			outTangent:Vector.<Vector.<Point>>)
		{
			_name           = name;
			_type           = type;
			_animation      = animation;
			_times          = times;
			_params         = params;
			_matrices       = matrices;
			_interpolations = interpolations;
			_inTangent      = inTangent;
			_outTangent     = outTangent;
			
			_length = _times.length;
			
			super();
		}
		
		override protected function initialize():void
		{
			Debug.assert(_times && _times.length, "no times!");
			Debug.assert( ( _type != TRANSFORM && (_params && _params.length) ) || (_matrices && _matrices.length), "no params!");
			
			var i:uint;
			var j:uint;
			var length:uint;
			var position:uint;
			var stride:uint;
			var rawData:Vector.<Number>;
			
			timesPtr    = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.floatTypeSize);
			
			position = timesPtr;
			stride   = Library.floatTypeSize;
			for(i = 0; i < _length; i ++)
			{
				Library.memory.position = position;
				Library.memory.writeFloat(_times[i]);
				
				position += stride;
			}
			
			if(_params && _params.length)
			{
				paramsPtr       = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.intTypeSize);
				paramsValuesPtr = Library.alchemy3DLib.applyForTmpBuffer( 
					_length * Library.floatTypeSize * ( length = 
					( (_type == TRANSLATION || _type == ROTATION || _type == SCALE) ? 3 : 1 ) ) );

				position = paramsValuesPtr;
				stride   = length * Library.floatTypeSize;
				for(i = 0; i < _length; i ++)
				{
					Library.memory.position = position;
					for(j = 0; j < length; j ++)
						Library.memory.writeFloat(_params[i][j]);
					
					Library.memory.position = paramsPtr + i * Library.intTypeSize;
					Library.memory.writeUnsignedInt(position);
					
					position += stride;
				}
			}
			else
				paramsPtr = NULL;
			
			if(_matrices && _matrices.length)
			{
				matricesPtr = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.floatTypeSize * 16);
				
				position = matricesPtr;
				stride   = Library.floatTypeSize * 16;
				for(i = 0; i < _length; i ++)
				{
					rawData = _matrices[i].rawData;
					Library.memory.position = position;
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
					
					position += stride;
				}
			}
			else
				matricesPtr = NULL;
			
			_pointer = Library.alchemy3DLib.initializeSkeletalChannel(
				_type, 
				_length,
				_animation ? _animation.getAnimationPointer() : NULL,
				timesPtr,
				paramsPtr,
				matricesPtr,
				NULL,
				NULL,
				NULL);
		}
		
		static public function serialize(input:SkeletalAnimation, data:ByteArray):void
		{
			var i:uint;
			var j:uint;
			var length:uint;
			data.writeUTF(input._name);
			data.writeInt(input._type);
			data.writeUnsignedInt(input._length);
			
			for(i = 0; i < input._length; i ++)
				data.writeFloat(input._times[i]);
			
			if(input._interpolations && input._interpolations.length)
			{
				data.writeBoolean(true);
				for(i = 0; i < input._length; i ++)
					data.writeFloat(input._interpolations[i]);
			}
			else
				data.writeBoolean(false);
				
			if(input._params && input._params.length)
			{
				data.writeBoolean(true);
				
				for(i = 0; i < input._length; i ++)
				{
					data.writeUnsignedInt(length = input._params[i].length);
						
					for(j = 0; j < length; j ++)
						data.writeFloat(input._params[i][j]);
				}
			}
			else
				data.writeBoolean(false);
			
			if(input._matrices && input._matrices.length)
			{
				data.writeBoolean(true);
					
				for(i = 0; i < input._length; i ++)
				{
					data.writeFloat(input._matrices[i].rawData[0]);
					data.writeFloat(input._matrices[i].rawData[1]);
					data.writeFloat(input._matrices[i].rawData[2]);
					data.writeFloat(input._matrices[i].rawData[3]);
					
					data.writeFloat(input._matrices[i].rawData[4]);
					data.writeFloat(input._matrices[i].rawData[5]);
					data.writeFloat(input._matrices[i].rawData[6]);
					data.writeFloat(input._matrices[i].rawData[7]);
					
					data.writeFloat(input._matrices[i].rawData[8]);
					data.writeFloat(input._matrices[i].rawData[9]);
					data.writeFloat(input._matrices[i].rawData[10]);
					data.writeFloat(input._matrices[i].rawData[11]);
					
					data.writeFloat(input._matrices[i].rawData[12]);
					data.writeFloat(input._matrices[i].rawData[13]);
					data.writeFloat(input._matrices[i].rawData[14]);
					data.writeFloat(input._matrices[i].rawData[15]);
				}
			}
			else
				data.writeBoolean(false);

			if(input._inTangent && input._inTangent.length)
			{
				data.writeBoolean(true);
				
				for(i = 0; i < input._length; i ++)
				{
					data.writeUnsignedInt(length = input._inTangent[i].length);
					
					for(j = 0; j < length; j ++)
					{
						data.writeFloat(input._inTangent[i][j].x);
						data.writeFloat(input._inTangent[i][j].y);
					}
				}
			}
			else
				data.writeBoolean(false);
			
			if(input._outTangent && input._outTangent.length)
			{
				data.writeBoolean(true);
				
				for(i = 0; i < input._length; i ++)
				{
					data.writeUnsignedInt(length = input._outTangent[i].length);
					
					for(j = 0; j < length; j ++)
					{
						data.writeFloat(input._outTangent[i][j].x);
						data.writeFloat(input._outTangent[i][j].y);
					}
				}
			}
			else
				data.writeBoolean(false);
		}
		
		static public function unserialize(data:ByteArray):SkeletalAnimation
		{
			var name:String = data.readUTF();
			var type:int    = data.readInt();
			var length:int  = data.readUnsignedInt();
			
			var times:Vector.<Number>;
			var interpolations:Vector.<Number>;
			var params:Vector.<Vector.<Number>>;
			var matrices:Vector.<Matrix3D>;
			var inTangent:Vector.<Vector.<Point>>;
			var outTangent:Vector.<Vector.<Point>>;
			
			var rawData:Vector.<Number>;
			
			var stride:uint;
			
			var i:uint;
			var j:uint;
			
			times = new Vector.<Number>();
			for(i = 0; i < length; i ++)
				times.push( data.readFloat() );
			
			if( data.readBoolean() )
			{
				interpolations = new Vector.<Number>();
				
				for(i = 0; i < length; i ++)
					interpolations.push( data.readFloat() );
			}
			
			if( data.readBoolean() )
			{
				params = new Vector.<Vector.<Number>>();
				
				for(i = 0; i < length; i ++)
				{
					stride = data.readUnsignedInt();
					
					params.push( new Vector.<Number>() );
					for(j = 0; j < stride; j ++)
						params[i].push( data.readFloat() );
				}
			}
			
			if( data.readBoolean() )
			{
				matrices = new Vector.<Matrix3D>();
				
				for(i = 0; i < length; i ++)
				{
					rawData = new Vector.<Number>();
					
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
					
					matrices.push( new Matrix3D(rawData) );
				}
			}
			
			if( data.readBoolean() )
			{
				inTangent = new Vector.<Vector.<Point>>();
				
				for(i = 0; i < length; i ++)
				{
					stride = data.readUnsignedInt();
					
					inTangent.push( new Vector.<Point>() );
					for(j = 0; j < stride; j ++)
						inTangent[i].push( new Point( data.readFloat(), data.readFloat() ) );
				}
			}
			
			if( data.readBoolean() )
			{
				outTangent = new Vector.<Vector.<Point>>();
				
				for(i = 0; i < length; i ++)
				{
					stride = data.readUnsignedInt();
					
					outTangent.push( new Vector.<Point>() );
					for(j = 0; j < stride; j ++)
						outTangent[i].push( new Point( data.readFloat(), data.readFloat() ) );
				}
			}
			
			return new SkeletalAnimation(name, type, null, times, params, matrices, interpolations, inTangent, outTangent);
		}
		
		public override function destroy(all:Boolean):void
		{
			_times          = null;
			_interpolations = null;
			_params         = null;
			_matrices       = null;
			_inTangent      = null;
			_outTangent     = null;
			
			Library.alchemy3DLib.freeTmpBuffer(timesPtr);
			Library.alchemy3DLib.freeTmpBuffer(paramsPtr);
			Library.alchemy3DLib.freeTmpBuffer(matricesPtr);
			Library.alchemy3DLib.freeTmpBuffer(interpolationsPtr);
			Library.alchemy3DLib.freeTmpBuffer(inTangentPtr);
			Library.alchemy3DLib.freeTmpBuffer(outTangentPtr);
			Library.alchemy3DLib.freeTmpBuffer(paramsValuesPtr);
			
			Library.alchemy3DLib.destroySkeletalChannel(_pointer);
		}
		
		private var timesPtr:uint;
		private var paramsPtr:uint;
		private var matricesPtr:uint;
		private var interpolationsPtr:uint;
		private var inTangentPtr:uint;
		private var outTangentPtr:uint;
		
		private var paramsValuesPtr:uint;

		internal var _name:String;
		internal var _type:int;
		internal var _length:uint;
		internal var _animation:AnimationObject;
		internal var _times:Vector.<Number>;
		internal var _interpolations:Vector.<Number>;
		internal var _params:Vector.<Vector.<Number>>;
		internal var _matrices:Vector.<Matrix3D>;
		internal var _inTangent:Vector.<Vector.<Point>>;
		internal var _outTangent:Vector.<Vector.<Point>>;
	}
}