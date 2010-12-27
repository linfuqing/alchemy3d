package AlchemyLib.animation.bones
{
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.container.Entity;
	import AlchemyLib.events.ContainerEvent;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.geom.Vertex3D;
	import AlchemyLib.tools.Debug;
	
	import flash.geom.Matrix3D;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	public class SkinMeshController extends Pointer
	{
		public function SkinMeshController(
			weights:Vector.<Vector.<Weight>>,
			vertices:Vector.<Vertex3D>,
			boneOffsetMatrices:Vector.<Matrix3D>,
			jointNames:Vector.<String>)
		{
			_numBones               = Math.min(weights.length, boneOffsetMatrices.length, jointNames.length);
			
			this.weights            = weights;
			this.vertices           = vertices;
			this.boneOffsetMatrices = boneOffsetMatrices;
			//this.bones              = bones;
			this.jointNames         = jointNames;

			bones = new Vector.<Entity>(_numBones);
			
			super();
			
			addEventListener(ContainerEvent.ADDED_TO_ENTITY, onAddedToEntity);
		}
		
		private function onAddedToEntity(e:ContainerEvent):void
		{
			var entity:Entity = e.container as Entity;
			
			if(entity && entity != _parent && entity.mesh && entity.mesh.skinMeshController == this)
			{
				_parent = entity;
				
				fillBones(entity);
				
				addEventListener(ContainerEvent.REMOVED_FROM_ENTITY, onRemovedFromEntity);
			}
		}
		
		private function onRemovedFromEntity(e:ContainerEvent):void
		{
			var entity:Entity = e.container as Entity;
			
			if(entity && _parent == entity && entity.mesh && entity.mesh.skinMeshController == null)
			{
				_parent = null;
				
				var length:uint = bones.length;
				for(var i:uint = 0; i < length; i ++)
					bones[i] = null;
				
				Library.memory.position = numBonesPtr;
				Library.memory.writeInt(0);
				
				removeEventListener(ContainerEvent.REMOVED_FROM_ENTITY, onRemovedFromEntity);
			}
		}
		
		static public function serialize(input:SkinMeshController, data:ByteArray):void
		{
			if(!input || !data)
			{
				Debug.warning("no input.");
				
				return;
			}
			
			data.writeUnsignedInt(input._numBones);
			
			var i:uint;
			var j:uint;
			var vertex:Vertex3D;
			var length:uint = input.vertices.length;
			
			data.writeUnsignedInt(length);
			for(i = 0; i < length; i ++)
			{
				vertex = input.vertices[i];
				data.writeFloat(vertex.x);
				data.writeFloat(vertex.y);
				data.writeFloat(vertex.z);
			}
			
			for(i = 0; i < input._numBones; i ++)
			{
				length = input.weights[i] ? input.weights[i].length : 0;
				data.writeUnsignedInt(length);
				for(j = 0; j < length; j ++)
				{
					if(input.weights[i][j])
					{
						data.writeBoolean(true);
						Weight.serialize(input.weights[i][j], data);
					}
					else
						data.writeBoolean(false);
				}
			}
			
			var rawData:Vector.<Number>;
			for(i = 0; i < input._numBones; i ++)
			{
				rawData = input.boneOffsetMatrices[i].rawData;
				
				data.writeFloat(rawData[0]);
				data.writeFloat(rawData[1]);
				data.writeFloat(rawData[2]);
				data.writeFloat(rawData[3]);
				
				data.writeFloat(rawData[4]);
				data.writeFloat(rawData[5]);
				data.writeFloat(rawData[6]);
				data.writeFloat(rawData[7]);
				
				data.writeFloat(rawData[8]);
				data.writeFloat(rawData[9]);
				data.writeFloat(rawData[10]);
				data.writeFloat(rawData[11]);
				
				data.writeFloat(rawData[12]);
				data.writeFloat(rawData[13]);
				data.writeFloat(rawData[14]);
				data.writeFloat(rawData[15]);
			}
			
			for(i = 0; i < input._numBones; i ++)
				data.writeUTF(input.jointNames[i]);
		}
		
		static public function unserialize(data:ByteArray):SkinMeshController
		{
			if(!data)
			{
				Debug.warning("no data.");
				
				return null;
			}
			
			var i:uint;
			var j:uint;

			var vertices:Vector.<Vertex3D> = new Vector.<Vertex3D>();
			var weights:Vector.<Vector.<Weight>> = new Vector.<Vector.<Weight>>();
			var boneOffsetMatrices:Vector.<Matrix3D> = new Vector.<Matrix3D>();
			var jointNames:Vector.<String> = new Vector.<String>();
			
			var numBones:uint = data.readUnsignedInt();
			var numVertices:uint = data.readUnsignedInt();
			
			for(i = 0; i < numVertices; i ++)
				vertices.push(new Vertex3D( data.readFloat(), data.readFloat(), data.readFloat() ) );
			
			for(i = 0; i < numBones; i ++)
			{
				weights.push( ( numVertices = data.readUnsignedInt() ) ? new Vector.<Weight>() : null );
				
				for(j = 0; j < numVertices; j ++)
					weights[i].push(data.readBoolean() ? Weight.unserialize(data) : null);
			}
			
			var rawData:Vector.<Number>;
			for(i = 0; i < numBones; i ++)
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
				
				boneOffsetMatrices.push( new Matrix3D(rawData) );
			}
			
			for(i = 0; i < numBones; i ++)
				jointNames.push(data.readUTF());
			
			return new SkinMeshController(weights, vertices, boneOffsetMatrices, jointNames);
		}
		
		override public function destroy(all:Boolean):void
		{
			if(!_pointer)
				return;
			
			weights             = null;
			vertices            = null;
			boneOffsetMatrices  = null;
			jointNames          = null;
			_weightsLengthArray = null;
			weightsPtrArray     = null;
			
			Library.alchemy3DLib.freeTmpBuffer(verticesPtr);
			Library.alchemy3DLib.freeTmpBuffer(weightsPtr);
			Library.alchemy3DLib.freeTmpBuffer(weightsLengthPtr);
			Library.alchemy3DLib.freeTmpBuffer(boneOffsetMatricesPtr);
			Library.alchemy3DLib.freeTmpBuffer(bonesPtr);
			
			Library.alchemy3DLib.destroySkinMeshController(_pointer);
			
			_pointer = NULL;
		}
		
		override protected function initialize():void
		{
			var i       :uint;
			var j       :uint;
			var length  :uint;
			
			var rawData:Vector.<Number>;
			
			_weightsLengthArray = new Vector.<uint>();
			_weightsLength      = 0;
			
			for(i = 0; i < _numBones; i ++)
			{
				_weightsLengthArray.push(weights[i] ? weights[i].length : 0);
				_weightsLength += _weightsLengthArray[i];
			}
			
			length = vertices.length;

			verticesPtr           = Library.alchemy3DLib.applyForTmpBuffer(length    * Library.floatTypeSize *  4);
			weightsPtr            = Library.alchemy3DLib.applyForTmpBuffer(_numBones * Library.intTypeSize       );
			weightsLengthPtr      = Library.alchemy3DLib.applyForTmpBuffer(_numBones * Library.intTypeSize       );
			boneOffsetMatricesPtr = Library.alchemy3DLib.applyForTmpBuffer(_numBones * Library.floatTypeSize * 16);
			bonesPtr              = Library.alchemy3DLib.applyForTmpBuffer(_numBones * Library.intTypeSize       );

			//vertices
			for(i = 0; i < length; i ++)
			{
				Library.memory.position = verticesPtr + Library.floatTypeSize *  4 * i;
				Library.memory.writeFloat(vertices[i].x);
				Library.memory.writeFloat(vertices[i].y);
				Library.memory.writeFloat(vertices[i].z);
				Library.memory.writeFloat(1);
			}
			
			weightsPtrArray = new Vector.<uint>();
			
			for(i = 0; i < _numBones; i ++)
			{
				length = _weightsLengthArray[i];
				
				//weights
				weightsPtrArray.push(
					length ? Library.alchemy3DLib.applyForTmpBuffer(length * Library.floatTypeSize + length * Library.intTypeSize) : NULL);
				
				Library.memory.position = weightsPtr  + Library.intTypeSize * i;
				Library.memory.writeUnsignedInt(weightsPtrArray[i]);
				
				Library.memory.position = weightsPtrArray[i];
				for(j = 0; j < length; j ++)
				{
					Library.memory.writeFloat(weights[i][j].weight);
					Library.memory.writeInt(weights[i][j].index);
				}
				
				//weightsLength
				Library.memory.position = weightsLengthPtr  + Library.intTypeSize * i;
				Library.memory.writeInt(length);
				
				//boneOffsetMatrices
				rawData = boneOffsetMatrices[i].rawData;

				Library.memory.position = boneOffsetMatricesPtr + Library.floatTypeSize * 16 * i;
				
				Library.memory.writeFloat(rawData[0 ]);
				Library.memory.writeFloat(rawData[1 ]);
				Library.memory.writeFloat(rawData[2 ]);
				Library.memory.writeFloat(rawData[3 ]);
				
				Library.memory.writeFloat(rawData[4 ]);
				Library.memory.writeFloat(rawData[5 ]);
				Library.memory.writeFloat(rawData[6 ]);
				Library.memory.writeFloat(rawData[7 ]);
				
				Library.memory.writeFloat(rawData[8 ]);
				Library.memory.writeFloat(rawData[9 ]);
				Library.memory.writeFloat(rawData[10]);
				Library.memory.writeFloat(rawData[11]);
				
				Library.memory.writeFloat(rawData[12]);
				Library.memory.writeFloat(rawData[13]);
				Library.memory.writeFloat(rawData[14]);
				Library.memory.writeFloat(rawData[15]);
				
				//bones
				//Library.memory.position = bonesPtr + Library.intTypeSize * i;
				//Library.memory.writeUnsignedInt(bones[i].pointer);
			}
			
			var ps:Array = Library.alchemy3DLib.initializeSkinMeshController(0, weightsPtr, weightsLengthPtr, verticesPtr, boneOffsetMatricesPtr, bonesPtr);
			
			_pointer    = ps[0];
			numBonesPtr = ps[1];
		}
		
		private function fillBones(source:Entity):Boolean
		{
			var i:uint;
			
			Library.memory.position = bonesPtr;
			for(i = 0; i < _numBones; i ++)
			{
				if( !( bones[i] = source.getRelative(jointNames[i]) ) )
				{
					Debug.warning("fill bones fail, joint name : " + jointNames[i] + " can not match.");
					break;
				}
				
				Library.memory.writeUnsignedInt(bones[i].pointer);
			}
			
			Library.memory.position = numBonesPtr;
			if(i == _numBones)
			{
				Debug.log("fill bones success!");
				
				Library.memory.writeInt(_numBones);
				
				return true;
			}
			
			Library.memory.writeInt(0);
			
			return false;
		}

		private var weights:Vector.<Vector.<Weight>>;
		private var vertices:Vector.<Vertex3D>;
		private var boneOffsetMatrices:Vector.<Matrix3D>;
		private var jointNames:Vector.<String>;
		
		private var bones:Vector.<Entity>;
		
		private var _numBones:uint;
		
		private var _weightsLength     :uint;
		private var _weightsLengthArray:Vector.<uint>;
		
		private var weightsPtr:uint;
		private var weightsLengthPtr:uint;
		private var weightsPtrArray:Vector.<uint>;
		
		private var verticesPtr:uint;
		private var boneOffsetMatricesPtr:uint;
		private var bonesPtr:uint;
		
		private var numBonesPtr:uint;
		
		private var _parent:Entity;
	}
}