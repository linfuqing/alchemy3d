package AlchemyLib.geom
{

	import AlchemyLib.animation.bones.SkinMeshController;
	import AlchemyLib.base.Library;
	import AlchemyLib.base.Pointer;
	import AlchemyLib.container.Entity;
	import AlchemyLib.container.SceneContainer;
	import AlchemyLib.events.ContainerEvent;
	import AlchemyLib.render.Material;
	import AlchemyLib.render.Texture;
	import AlchemyLib.tools.Debug;
	
	import __AS3__.vec.Vector;
	
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	
	public class Mesh3D extends Pointer
	{
		public static const MATERIAL_KEY:uint		= 0x01;
		public static const TEXTURE_KEY:uint		= 0x02;
		public static const FOG_KEY:uint			= 0x04;
		public static const LIGHT_KEY:uint			= 0x08;
		public static const RENDERMODE_KEY:uint		= 0x10;
		public static const ALPHA_KEY:uint			= 0x20;
		
		public static const ADDRESS_MODE_NONE:uint		= 0x01;
		public static const ADDRESS_MODE_WRAP:uint		= 0x02;
		public static const ADDRESS_MODE_BORDER:uint	= 0x04;
		public static const ADDRESS_MODE_CLAMP:uint		= 0x08;
		public static const ADDRESS_MODE_MIRROR:uint	= 0x10;
		
		public var doubleSide:Boolean = false;
		
		public function get skinMeshController():SkinMeshController
		{
			return _skinMeshController;
		}
		
		public function set skinMeshController(controller:SkinMeshController):void
		{
			var temp:SkinMeshController = controller;
			if(temp && temp != _skinMeshController)
			{
				_skinMeshController = null;
				
				if(removeEvent)
					temp.dispatchEvent(removeEvent);
			}
			
			_skinMeshController = controller;
			
			if(controller)
			{
				if(_parent)
					controller.dispatchEvent(addEvent);
				
				Library.memory.position = skinMeshControllerPtr;
			
				Library.memory.writeUnsignedInt(controller.pointer);
			}
		}
		
		public function get vertices():Vector.<Vertex3D>
		{
			return _vertices.concat();
		}
		
		public function get faces():Vector.<Triangle3D>
		{
			return _faces.concat();
		}
		
		public function get numVertices():uint
		{
			return _vertices ? _vertices.length : 0;
		}
		
		public function get numFaces():uint
		{
			return _faces ? _faces.length : 0;
		}
		
		public function get isHit():Boolean
		{
			Library.memory.position = hitPointer;
			
			return Library.memory.readInt() ? true : false;
		}
		
		public function set texRotation(value:Number):void
		{
			Library.memory.position = texRotationPointer;
			Library.memory.writeFloat(value);
			
			Library.memory.position = texTransformDirtyPointer;
			Library.memory.writeBoolean(true);
		}
		
		public function get texRotation():Number
		{
			Library.memory.position = texRotationPointer;
			return Library.memory.readFloat();
		}
		
		public function set texScaleX(value:Number):void
		{
			Library.memory.position = texScalePointer;
			Library.memory.writeFloat(value);
			
			Library.memory.position = texTransformDirtyPointer;
			Library.memory.writeBoolean(true);
		}
		
		public function get texScaleX():Number
		{
			Library.memory.position = texScalePointer;
			return Library.memory.readFloat();
		}
		
		public function set texScaleY(value:Number):void
		{
			Library.memory.position = texScalePointer + Library.floatTypeSize;
			Library.memory.writeFloat(value);
			
			Library.memory.position = texTransformDirtyPointer;
			Library.memory.writeBoolean(true);
		}
		
		public function get texScaleY():Number
		{
			Library.memory.position =  texScalePointer + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set texOffsetX(value:Number):void
		{
			Library.memory.position = texOffsetPointer;
			Library.memory.writeFloat(value);
			
			Library.memory.position = texTransformDirtyPointer;
			Library.memory.writeBoolean(true);
		}
		
		public function get texOffsetX():Number
		{
			Library.memory.position = texOffsetPointer;
			return Library.memory.readFloat();
		}
		
		public function set texOffsetY(value:Number):void
		{
			Library.memory.position = texOffsetPointer + Library.floatTypeSize;
			Library.memory.writeFloat(value);
			
			Library.memory.position = texTransformDirtyPointer;
			Library.memory.writeBoolean(true);
		}
		
		public function get texOffsetY():Number
		{
			Library.memory.position =  texOffsetPointer + Library.floatTypeSize;
			return Library.memory.readFloat();
		}
		
		public function set addressMode(value:int):void
		{
			Library.memory.position = addressModePointer;
			Library.memory.writeInt(value);
		}
		
		public function get addressMode():int
		{
			Library.memory.position = addressModePointer;
			return Library.memory.readUnsignedInt();
		}
		
		public function get useMipmap():Boolean
		{
			Library.memory.position = useMipmapPointer;
			return Library.memory.readBoolean();
		}
		
		public function set useMipmap(bool:Boolean):void
		{
			Library.memory.position = useMipmapPointer;
			Library.memory.writeBoolean(bool);
		}
		
		public function get mipDist():Number
		{
			Library.memory.position = mipDistPointer;
			return Library.memory.readFloat();
		}
		
		public function set mipDist(value:Number):void
		{
			Library.memory.position = mipDistPointer;
			Library.memory.writeFloat(value);
		}
		
		public function set octreeDepth(value:uint):void
		{
			Library.memory.position = octreeDepthPointer;
			Library.memory.writeInt(value);
		}
		
		public function get fogEnable():Boolean
		{
			Library.memory.position = fogEnablePointer;
			return Library.memory.readBoolean();
		}
		
		public function set fogEnable(bool:Boolean):void
		{
			Library.memory.position = fogEnablePointer;
			Library.memory.writeBoolean(bool);
		}
		
		public function get lightEnable():Boolean
		{
			return _lightEnable;
		}
		
		public function set lightEnable(bool:Boolean):void
		{
			_lightEnable = bool;
			
			Library.memory.position = lightEnablePointer;
			Library.memory.writeBoolean(bool);
		}
		
		public function Mesh3D( vertices:Vector.<Vertex3D> = null, faces:Vector.<Triangle3D> = null )
		{	
			this._lightEnable = false;
			
			_vertices  = vertices;
			_faces     = faces;
			
			_skinMeshController = null;
			
			super();
			
			addEventListener(ContainerEvent.ADDED_TO_ENTITY, onAddedToEntity);
		}
		
		static public function serialize(input:Mesh3D, data:ByteArray):void
		{
			if(!input || !data)
			{
				Debug.warning("no input");
				
				return;
			}
			
			if( !(input._vertices && input._vertices.length && input._faces && input._faces.length) )
			{
				Debug.warning("empty mesh");
				
				return;
			}
			
			var i:uint;
			var length:uint;
			
			data.writeUnsignedInt(length = input._vertices.length);
			for(i = 0; i < length; i ++)
				Vertex3D.serialize(input._vertices[i], data);
			
			data.writeUnsignedInt(length = input._faces.length);
			for(i = 0; i < length; i ++)
			{
				if(input._faces[i]._material)
				{
					data.writeBoolean(true);
					data.writeUTF(input._faces[i]._material.name ? input._faces[i]._material.name : "");
				}
				else
					data.writeBoolean(false);
				
				if(input._faces[i]._texture)
				{
					data.writeBoolean(true);
					data.writeUTF(input._faces[i]._texture.name ? input._faces[i]._texture.name : "");
				}
				else
					data.writeBoolean(false);
				
				Triangle3D.serialize(input._faces[i], data);
			}
			
			if(input._skinMeshController)
			{
				data.writeBoolean(true);
				SkinMeshController.serialize(input._skinMeshController, data);
			}
			else
				data.writeBoolean(false);
			
			if(input._parent)
			{
				data.writeBoolean(true);
				
				data.writeUTF( input._parent.name ? input._parent.name : ( input._parent.id ? input._parent.id : (input._parent.sid ? input._parent.sid : "") ) );
			}
			else
				data.writeBoolean(false);
		}
		
		static public function unserialize(data:ByteArray, material:Array, texture:Array, node:Entity):Mesh3D
		{
			if(!data)
			{
				Debug.warning("no data");
				
				return null;
			}
			
			var vertices:Vector.<Vertex3D> = new Vector.<Vertex3D>();
			var faces:Vector.<Triangle3D> = new Vector.<Triangle3D>();
			
			var i:uint;
			var length:uint;
			
			length = data.readUnsignedInt();
			for(i = 0; i < length; i ++)
				vertices.push( Vertex3D.unserialize(data) );
			
			length = data.readUnsignedInt();
			
			var materialName:String;
			var textureName:String;
			
			if(material && texture)
			{
				for(i = 0; i < length; i ++)
				{
					materialName = data.readBoolean() ? data.readUTF() : null;
					textureName  = data.readBoolean() ? data.readUTF() : null;
					
					faces.push( Triangle3D.unserialze(
						data, 
						material[materialName] as Material, 
						texture[textureName] as Texture ) );
				}
			}
			else if(material)
			{
				for(i = 0; i < length; i ++)
				{
					materialName = data.readBoolean() ? data.readUTF() : null;
					textureName  = data.readBoolean() ? data.readUTF() : null;
					
					faces.push( Triangle3D.unserialze(
						data, 
						material[materialName] as Material, 
						null) );
				}
			}
			else if(texture)
			{
				for(i = 0; i < length; i ++)
				{
					materialName = data.readBoolean() ? data.readUTF() : null;
					textureName  = data.readBoolean() ? data.readUTF() : null;
					
					faces.push( Triangle3D.unserialze(
						data, 
						null, 
						texture[textureName] as Texture) );
				}
			}
			else
			{
				for(i = 0; i < length; i ++)
				{
					materialName = data.readBoolean() ? data.readUTF() : null;
					textureName  = data.readBoolean() ? data.readUTF() : null;
					
					faces.push( Triangle3D.unserialze(
						data, 
						null, 
						null) );
				}
			}
			
			var mesh:Mesh3D = new Mesh3D(vertices, faces);
			
			if( data.readBoolean() )
				mesh.skinMeshController = SkinMeshController.unserialize(data);
			
			if( data.readBoolean() )
			{
				var info:String = data.readUTF();
				
				if(info && node)
				{
					if(info == node.name || info == node.id || info == node.sid)
						node.mesh = mesh;
					else
					{
						var parent:Entity = node.getChild(info);
						
						if(parent)
							parent.mesh = mesh;
					}
				}
			}
			
			return mesh;
		}
		
		public override function destroy(all:Boolean):void
		{
			if(!_pointer)
				return;
			
			_vertices = null
			_faces    = null;
			
			Library.alchemy3DLib.destroyMesh(_pointer);
			
			if(all)
			{
				if(_skinMeshController)
					_skinMeshController.destroy(true);
			}
			
			_pointer = NULL;
		}
		
		public function clone(material:Material = null, texture:Texture = null, renderMode:int = -1):Mesh3D
		{
			isCreated = false;
			
			var mesh:Mesh3D = new Mesh3D();
			
			var ps:Array = Library.alchemy3DLib.cloneMesh(_pointer, material ? material.pointer : NULL, texture ? texture.pointer : NULL, renderMode);
			
			mesh.setPointers(ps);
			
			isCreated = true;
			
			return mesh;
		}
		
		public function setAttribute(key:uint, value:*):void
		{
			var intValue:int;
			var length:uint;
			var i:uint;
			
			switch(key)
			{
				case MATERIAL_KEY:
					var material:Material = value as Material;
					
					Debug.assert(material != null, "error value");
					
					if( _faces && (length = _faces.length) )
						for(i = 0; i < length; i ++)
							_faces[i]._material = material;
					
					intValue = material.pointer;
					
					break;
				
				case TEXTURE_KEY:
					var texture:Texture = value as Texture;
					
					Debug.assert(texture != null, "error value");
					
					if( _faces && (length = _faces.length) )
						for(i = 0; i < length; i ++)
							_faces[i]._texture = texture;
					
					intValue = texture.pointer;
					
					break;
				
				case RENDERMODE_KEY:
					var renderMode:uint = value;
					
					if( _faces && (length = _faces.length) )
						for(i = 0; i < length; i ++)
							_faces[i].render_mode = renderMode;
				
					intValue = renderMode;
					
					break;
				
				default:
					
					intValue = value is Boolean ? (value ? TRUE : FALSE) : value;
					break;
			}
			
			Library.alchemy3DLib.setMeshAttribute( _pointer, key, intValue );
		}
		
		
		override protected function initialize():void
		{
			var p  :Vector.<uint> = setFaces( _faces );
			var vp :uint          = setVertices( _vertices ),
				
				fp :uint          = p[0],
				uvp:uint          = p[1],
				mp :uint          = p[2],
				tp :uint          = p[3],
				rmp:uint          = p[4];
			
			
			var ps:Array = Library.alchemy3DLib.initializeMesh( vp, fp, uvp, mp, tp, rmp,  numVertices, numFaces );
			
			Library.alchemy3DLib.freeTmpBuffer(vp);
			Library.alchemy3DLib.freeTmpBuffer(fp);
			Library.alchemy3DLib.freeTmpBuffer(mp);
			Library.alchemy3DLib.freeTmpBuffer(tp);
			Library.alchemy3DLib.freeTmpBuffer(rmp);
			
			setPointers(ps);
		}
		
		private function setPointers(ps:Array):void
		{
			_pointer            		= ps[0];
			lightEnablePointer			= ps[1];
			fogEnablePointer			= ps[2];
			useMipmapPointer			= ps[3];
			mipDistPointer				= ps[4];
			vDirtyPointer				= ps[5];
			octreeDepthPointer			= ps[6];
			addressModePointer			= ps[7];
			texRotationPointer			= ps[8];
			texOffsetPointer			= ps[9];
			texScalePointer				= ps[10];
			texTransformDirtyPointer	= ps[11];
			hitPointer                  = ps[12];
			skinMeshControllerPtr       = ps[13];
			
			if( ps[14] && _vertices )
			{
				Library.memory.position = ps[14];
				
				for( var i:uint = 0; i < _vertices.length; i ++ )
				{
					_vertices[i].setPointer( Library.memory.readUnsignedInt() );
				}
				
				Library.alchemy3DLib.freeTmpBuffer( ps[14] );
			}
		}
		
		private function setVertices( vs:Vector.<Vertex3D> ):uint
		{
			if( !vs )
			{
				return NULL;
			}
			
			var v     :Vertex3D; 
			
			var vl    :uint = vs.length;
			var vp    :uint = Library.alchemy3DLib.applyForTmpBuffer( vl * 3 * Library.floatTypeSize );
			
			Library.memory.position = vp;
			
			for each ( v in vs )
			{
				Library.memory.writeFloat( v._x );
				Library.memory.writeFloat( v._y );
				Library.memory.writeFloat( v._z );
			}
			
			return vp;
		}
		
		private function setFaces( fs:Vector.<Triangle3D> ):Vector.<uint>
		{
			if( !fs )
			{
				return Vector.<uint>([NULL, NULL, NULL, NULL, NULL]);
			}
			
			var fl    :uint = fs.length,
				
				fp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize       * 3 ),
				uvp   :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.floatTypeSize * 2 * 3 ),
				mp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           ),
				tp    :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           ),
				rmp   :uint = Library.alchemy3DLib.applyForTmpBuffer( fl * Library.intTypeSize           );
			
			Library.memory.position = fp;
			
			for each ( var f:Triangle3D in fs )
			{
				Debug.assert(f.v0 < _vertices.length && f.v1 < _vertices.length && f.v2 < _vertices.length, "index error");
				
				Library.memory.writeUnsignedInt(f.v0);
				Library.memory.writeUnsignedInt(f.v1);
				Library.memory.writeUnsignedInt(f.v2);
			}
			
			Library.memory.position = uvp;
			
			for each( f in fs )
			{
				Library.memory.writeFloat( f.uv0._u );
				Library.memory.writeFloat( f.uv0._v );
				Library.memory.writeFloat( f.uv1._u );
				Library.memory.writeFloat( f.uv1._v );
				Library.memory.writeFloat( f.uv2._u );
				Library.memory.writeFloat( f.uv2._v );
			}
			
			Library.memory.position = mp;
			
			for each( f in fs )
			{
				Library.memory.writeUnsignedInt( f._material.pointer );
			}
			
			Library.memory.position = tp;
			
			for each( f in fs )
			{
				Library.memory.writeUnsignedInt( f._texture ? f._texture.pointer : 0 );
			}
			
			Library.memory.position = rmp;
			
			for each( f in fs )
			{
				Library.memory.writeInt( f.render_mode );
			}
			
			return Vector.<uint>( [fp, uvp, mp, tp, rmp] );
		}
		
		private function onAddedToEntity(e:ContainerEvent):void
		{
			var entity:Entity = e.container as Entity;
			if(entity && entity.mesh == this && entity != _parent)
			{
				if(_parent)
					_parent.mesh = null;
				
				_parent = entity;
				
				addEvent    = new ContainerEvent(ContainerEvent.ADDED_TO_ENTITY    , _parent);
				removeEvent = new ContainerEvent(ContainerEvent.REMOVED_FROM_ENTITY, _parent);
				
				if(_skinMeshController)
					_skinMeshController.dispatchEvent(addEvent);
				
				addEventListener(ContainerEvent.REMOVED_FROM_ENTITY, onRemovedFromEntity);
			}
		}
		
		private function onRemovedFromEntity(e:ContainerEvent):void
		{
			var entity:Entity = e.container as Entity;
			if(entity && _parent == entity && !entity.mesh)
			{
				_parent = null;
				
				if(_skinMeshController)
					_skinMeshController.dispatchEvent(removeEvent);
				
				addEvent    = null;
				removeEvent = null;
				
				removeEventListener(ContainerEvent.REMOVED_FROM_ENTITY, onRemovedFromEntity);
			}
		}
		
		private var _vertices:Vector.<Vertex3D>;
		private var _faces:Vector.<Triangle3D>;
		
		private var lightEnablePointer:uint;
		private var fogEnablePointer:uint;
		private var useMipmapPointer:uint;
		private var mipDistPointer:uint;
		private var vDirtyPointer:uint;
		private var octreeDepthPointer:uint;
		private var addressModePointer:uint;
		private var texRotationPointer:uint;
		private var texOffsetPointer:uint;
		private var texScalePointer:uint;
		private var texTransformDirtyPointer:uint;
		private var hitPointer:uint;
		private var skinMeshControllerPtr:uint;
		
		private var _skinMeshController:SkinMeshController;
		
		private var _lightEnable:Boolean;
		
		private var _parent:Entity;
		
		private var addEvent:ContainerEvent;
		private var removeEvent:ContainerEvent;
	}
}