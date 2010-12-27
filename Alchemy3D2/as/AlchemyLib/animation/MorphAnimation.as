package AlchemyLib.animation
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	
	import flash.geom.Vector3D;
	
	public class MorphAnimation extends Pointer
	{
		public function MorphAnimation(
			target:AnimationObject,
			name:Vector.<String>,
			times:Vector.<int>, 
			vertices:Vector.<Vector.<Vector3D>>)
		{
			_target   = target;
			_name     = name;
			_times    = times;
			_vertices = vertices;
			
			if(name && times && vertices)
				_length = Math.min(name.length, times.length, vertices.length);
			else
				_length = 0;
			
			super();
		}
		
		public function toAnimation( animation:String ):void
		{
			if( animation )
			{
				Library.memory.position = this.currentFrameNamePointer;
				Library.memory.writeUTFBytes( animation );
				
				Library.memory.position = this.dirtyPointer;
				Library.memory.writeInt( TRUE );
				
				Library.memory.position = this.nameLengthPointer;
				Library.memory.writeInt( animation.length );
			}
		}
		
		override protected function initialize():void
		{
			if(_length)
			{
				namePointer           = Library.alchemy3DLib.applyForTmpBuffer(_length * FRAME_NAME_LENGTH    );
				timesPointer          = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.floatTypeSize);
				verticesPointer       = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.intTypeSize  );
				verticesLengthPointer = Library.alchemy3DLib.applyForTmpBuffer(_length * Library.intTypeSize  );
				
				verticesLengthArray   = new Vector.<Number>();
				verticesPointers      = new Vector.<uint>();
				
				var i:uint;
				var j:uint;
				var length:uint;
				var pointer:uint;
				
				for(i = 0; i < _length; i ++)
				{
					Library.memory.position = namePointer + FRAME_NAME_LENGTH * i;
					Library.memory.writeUTFBytes(_name[i]);
					
					Library.memory.position = timesPointer + Library.floatTypeSize * i;
					Library.memory.writeInt(_times[i]);
					
					length = _vertices[i].length;
					pointer = Library.alchemy3DLib.applyForTmpBuffer(length * Library.floatTypeSize * 4);
					Library.memory.position = verticesPointer;
					Library.memory.writeUnsignedInt(pointer);
					verticesPointers.push(pointer);
					
					Library.memory.position = pointer;
					for(j = 0; j < length; j ++)
					{
						Library.memory.writeFloat(_vertices[i][j].x);
						Library.memory.writeFloat(_vertices[i][j].y);
						Library.memory.writeFloat(_vertices[i][j].z);
						Library.memory.writeFloat(1);
					}
					
					Library.memory.position = verticesLengthPointer;
					Library.memory.writeUnsignedInt(length);
				}
			}
			else
			{
				namePointer     = NULL;
				timesPointer    = NULL;
				verticesPointer = NULL;
			}
			
			var ps:Array = Library.alchemy3DLib.initializeMorphChannel(_target ? _target.getAnimationPointer() : NULL, _length, namePointer, timesPointer, verticesPointer, verticesLengthPointer);
			
			_pointer                = ps[0];
			dirtyPointer            = ps[1];
			currentFrameNamePointer = ps[2];
			nameLengthPointer       = ps[3];
		}

		private var _target:AnimationObject;
		
		private var _length:uint;
		private var _name:Vector.<String>;
		private var _times:Vector.<int>;
		private var _vertices:Vector.<Vector.<Vector3D>>;
		private var verticesLengthArray:Vector.<uint>;
		private var verticesPointers:Vector.<uint>;
		
		private var namePointer:uint;
		private var timesPointer:uint;
		private var verticesPointer:uint;
		private var verticesLengthPointer:uint;
		
		private var dirtyPointer:uint;
		private var currentFrameNamePointer:uint;
		private var nameLengthPointer:uint;
		
		private const FRAME_NAME_LENGTH:uint = 16;
	}
}