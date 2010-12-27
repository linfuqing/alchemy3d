package AlchemyLib.external
{
	import AlchemyLib.animation.AnimationObject;
	import AlchemyLib.animation.SkeletalAnimation;
	import AlchemyLib.animation.bones.SkinMeshController;
	import AlchemyLib.animation.bones.Weight;
	import AlchemyLib.container.Entity;
	import AlchemyLib.events.LoadEvent;
	import AlchemyLib.geom.Mesh3D;
	import AlchemyLib.geom.TextureCoordinate;
	import AlchemyLib.geom.Triangle3D;
	import AlchemyLib.geom.Vertex3D;
	import AlchemyLib.render.Material;
	import AlchemyLib.render.RenderMode;
	import AlchemyLib.render.Texture;
	import AlchemyLib.tools.Debug;
	
	import flash.display.BitmapData;
	import flash.display.Loader;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.geom.*;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.*;
	
    /**
    * File loader for the Collada file format with animation.
    */
    public class DAE extends AnimationObject
    {
		public function set renderMode(value:uint):void
		{
			render_mode = value;
			
			if(geometryList)
			{
				for each(var entity:Entity in geometryList)
				{
					entity.mesh.setAttribute(Mesh3D.RENDERMODE_KEY, value);
				}
			}
		}

		public function get geometry():Vector.<Mesh3D>
		{
			var output:Vector.<Mesh3D> = new Vector.<Mesh3D>();
			
			for each(var mesh:Entity in geometryList)
				output.push(mesh.mesh);
				
			return output;
		}
		
		public function get material():Vector.<Material>
		{
			var output:Vector.<Material> = new Vector.<Material>();
			
			for each(var material:Material in materialList)
				output.push(material);
			
			return output;
		}
		
		public function get texture():Vector.<Texture>
		{
			var output:Vector.<Texture> = new Vector.<Texture>();
			
			for each(var texture:Texture in textureList)
				output.push(texture);
			
			return output;
		}
		
		public function get animation():Vector.<SkeletalAnimation>
		{
			var output:Vector.<SkeletalAnimation> = new Vector.<SkeletalAnimation>();
			
			for each(var channel:SkeletalAnimation in animationList)
				output.push(channel);
				
			return output;
		}
		
		/**
		 * Creates a new <code>Collada</code> object.
		 */
        public function DAE(autoPlay:Boolean = true, loop:Boolean = true, scaling:Number = 1, renderMode:uint = RenderMode.RENDER_WIREFRAME_TRIANGLE_32)
        {
            super(autoPlay, loop);

			_scaling = scaling;
			
			collada = null;
			
			render_mode = renderMode;
        }
		
		public function toAnimation(name:String):void
		{
			if(!collada)
			{
				Debug.warning("no data!");
				
				return;
			}
			
			var animationClip:Vector.<SkeletalAnimation> = animationClipList[name];
			
			setupSkeletalAnimation(animationClip);
		}
		
		public function getAnimationClipByName(name:String):Vector.<SkeletalAnimation>
		{
			if(!animationClipList)
			{
				Debug.warning("no data");
				return null;
			}
			
			var animation:Vector.<SkeletalAnimation> = animationClipList[name];
			
			return animation ? animation.concat() : null;
		}
		
		public function load(url:String, isParseAnimation:Boolean = true, isParseTexture:Boolean = true, material:Vector.<Material> = null, texture:Vector.<Texture> = null):void
		{
			if(isParseTexture)
			{
				var index:int = url.lastIndexOf('/');
				basicPath = -1 == index ? "" : url.slice(0, index);
			}
			else
				basicPath = null;
			
			this.isParseAnimation = isParseAnimation;
			
			if(material)
				setMaterialList(material);
			
			if(texture)
				setTextureList(texture);
			
			_loader = new URLLoader();
			_loader.dataFormat = URLLoaderDataFormat.BINARY;
			_loader.addEventListener(Event.COMPLETE, onLoadComplete);
			_loader.load(new URLRequest(url));
		}
		
		public function parse(data:*, isParseAnimationClips:Boolean = true, basicPath:String = "", material:Vector.<Material> = null, texture:Vector.<Texture> = null):void
		{
			if(material)
				setMaterialList(material);
			
			if(texture)
				setTextureList(texture);
			
			prepareData(data);
			
			if(isParseAnimationClips)
				parseAnimationClips();

			if(basicPath)
				parseTexture(basicPath);
		}
		
		public function parseTexture(basicPath:String = ""):void
		{
			var name:String;
			var texture:Texture;
			var j:uint;
			var length:uint;

			for(var i:String in textureFileName)
			{
				name = textureFileName[i];
				
				Debug.log("load texture : " + name);
				
				texture = getTexture(i);
				texture.addEventListener(Event.COMPLETE, onTextureLoadedComplete);
				texture.load(name);
			}
		}
		
		public function setBitmapData(data:Vector.<BitmapData>, map:Vector.<String>):void
		{
			var length:uint;
			
			length = data.length;
			for(var i:uint = 0; i < length; i ++)
				getTexture(map[i]).bitmapData = data[i];
		}
		
		public function setAttribute(key:uint, value:*):void
		{
			if(geometryList)
				for each(var entity:Entity in geometryList)
					entity.mesh.setAttribute(key, value);
		}
		
		private function onLoadComplete(e:Event):void
		{
			parse(_loader.data, isParseAnimation, basicPath);
		}
		
		private function onTextureLoadedComplete(e:Event):void
		{
			var bitmapData:BitmapData;
			var target:Texture;
			if( !(bitmapData = ( target = (e.target as Texture) ).bitmapData) )
			{
				if(render_mode != RenderMode.RENDER_WIREFRAME_TRIANGLE_32
				|| render_mode != RenderMode.RENDER_FLAT_TRIANGLE_INVZB_32
				|| render_mode != RenderMode.RENDER_GOURAUD_TRIANGLE_INVZB_32
				|| render_mode != RenderMode.RENDER_TRIANGLE_FSINVZB_ALPHA_32
				|| render_mode != RenderMode.RENDER_TRIANGLE_GSINVZB_ALPHA_32)
				{
					Debug.warning("load texture error, change to wireframe mode.");
				
					renderMode = RenderMode.RENDER_WIREFRAME_TRIANGLE_32;
				}
			}
			else
			{
				bitmapData.dispose();
				
				var isComplete:Boolean = true;
				for each(var texture:Texture in textureList)
					if( !(bitmapData = texture.bitmapData) )
						isComplete = false;
					else
						bitmapData.dispose();
					
				if(isComplete)
					dispatchEvent( new LoadEvent(LoadEvent.TEXTURE_COMPLETE) );
			}
			
			target.removeEventListener(Event.COMPLETE, onTextureLoadedComplete);
		}
        
		private function setMaterialList(input:Vector.<Material>):void
		{
			if(!input)
				return;
			
			if(!materialList)
				materialList = new Array();
			
			var length:uint = input.length;
			for(var i:uint = 0; i < length; i ++)
				materialList[input[i].name] = input[i];
		}
		
		private function setTextureList(input:Vector.<Texture>):void
		{
			if(!input)
				return;
			
			if(!textureList)
				textureList = new Array();
			
			var length:uint = input.length;
			for(var i:uint = 0; i < length; i ++)
				textureList[input[i].name] = input[i];
		}
		
        /** @private */
        private function prepareData(data:*):void
        {
        	// void junk byte, flash player bug
			try{
            	collada = XML(data);
   			}catch(e:*){
				Debug.log("Junk byte!?");
   				var _pos:int = String(data).indexOf("</COLLADA>"); 
  				collada = new XML(String(data).substring(0, _pos+String("</COLLADA>").length));
   			}
        	
			default xml namespace = collada.namespace();
			
			geometryList = new Vector.<Entity>();
			geometryName = new Vector.<String>();
			geometryXMLList = new Vector.<XML>();
			geometryControllerXMLList = new Vector.<XML>();
			
			materialList = materialList ? materialList : new Array();
			textureList = textureList ? textureList : new Array();
			textureFileName = new Array();

			animationList = new Array();
			animationClipList = new Array();
			
			_defaultTransform = new Array();
			
			Debug.log("------------- Begin Parse Collada -------------");

            // Get up axis
            yUp = (collada["asset"].up_axis == "Y_UP")||(String(collada["asset"].up_axis) == "");
			
            parseScene();
			
			for(var i:uint = 0; i < geometryList.length; i ++)
				parseMesh(
					geometryName[i],
					geometryXMLList[i],
					geometryControllerXMLList[i],
					geometryList[i]);
			
			dispatchEvent( new Event(Event.COMPLETE) );
        }
		
		private function parseScene():void
		{
			var scene:XML = collada["library_visual_scenes"].visual_scene.(@id == getId(collada["scene"].instance_visual_scene.@url))[0];
			
			if (scene == null) {
				Debug.log("------------- No scene to parse -------------");
				return;
			}
			
			Debug.log("------------- Begin Parse Scene -------------");
			
			for each (var node:XML in scene["node"])
				parseNode(node, this);
			
			Debug.log("------------- End Parse Scene -------------");
		}
		
		private function parseNode(node:XML, parent:Entity):void
		{	
			if (String(node["instance_light"].@url) != "" || String(node["instance_camera"].@url) != "")
				return;
			
			var entity:Entity;
			
			if (String(node["instance_controller"]) == "" && String(node["instance_geometry"]) == "")
			{
				
				if (String(node.@type) == "JOINT")
				{
					entity      = new Entity();
					entity.bone = true;
					entity.sid   = node.@sid;
				}
				else
				{
					if (String(node["instance_node"].@url) == "" && (String(node["node"]) == ""))
						return;
					
					entity = new Entity();
				}
			}
			else
				entity = new Entity();
			
			entity.name = String(node.@name);
			entity.id   = node.@id;
			
			var transform:Matrix3D = new Matrix3D();
			
			/*ColladaMaya 3.05B
			if (String(node.@type) == "JOINT")
				_objectData.id = node.@sid;
			else
				_objectData.id = node.@id;
			*/
			
			/* Deprecated for ColladaMaya 3.02
			if(String(node.@name) != "")
			{
			_objectData.name = String(node.@name);
			}else{
			_objectData.name = String(node.@id);
			}
			*/
			
			Debug.log("Parse Node : " + entity.id + " : " + entity.name);
			
			var nodeName:String;
			var geo:XML;
			var ctrlr:XML;
			var sid:String;
			var instance_material:XML;
			var arrayChild:Vector.<Number>;

			for each (var childNode:XML in node.children())
			{
				nodeName = String(childNode.name()["localName"]);
				
				Debug.log("Transform + " + nodeName);
				
				switch(nodeName)
				{
					case "translate":
						arrayChild = getArray(childNode);
						if (yUp)
							//transform.prependTranslation(-arrayChild[0]*_scaling, -arrayChild[1]*_scaling, arrayChild[2]*_scaling);
							transform.prependTranslation(-arrayChild[0]*_scaling, arrayChild[1]*_scaling, arrayChild[2]*_scaling);
						else
							//transform.prependTranslation(arrayChild[0]*_scaling, -arrayChild[2]*_scaling, arrayChild[1]*_scaling);
							transform.prependTranslation(arrayChild[0]*_scaling, arrayChild[2]*_scaling, arrayChild[1]*_scaling);
						
						break;
					
					case "rotate":
						arrayChild = getArray(childNode);
						/*sid = childNode.@sid;
						if (_objectData is BoneData && (sid == "rotateX" || sid == "rotateY" || sid == "rotateZ" || sid == "rotX" || sid == "rotY" || sid == "rotZ")) {
							if (yUp) {
								boneData.jointTransform.prependRotation(-arrayChild[3], new Vector3D(arrayChild[0], arrayChild[1], -arrayChild[2]));
								//boneData.jointTransform.prependRotation(arrayChild[3], new Vector3D(-arrayChild[0], arrayChild[1], arrayChild[2]));
							} else {
								boneData.jointTransform.prependRotation(arrayChild[3], new Vector3D(arrayChild[0], -arrayChild[2], arrayChild[1]));
								//boneData.jointTransform.prependRotation(arrayChild[3], new Vector3D(arrayChild[0], arrayChild[2], arrayChild[1]));
							}
						} else {*/
						if (yUp) 
							//transform.prependRotation(-arrayChild[3], new Vector3D(arrayChild[0], arrayChild[1], -arrayChild[2]));
							transform.prependRotation(-arrayChild[3], new Vector3D(-arrayChild[0], arrayChild[1], arrayChild[2]));
						else 
							//transform.prependRotation(arrayChild[3], new Vector3D(arrayChild[0], -arrayChild[2], arrayChild[1]));
							transform.prependRotation(-arrayChild[3], new Vector3D(arrayChild[0], arrayChild[2], arrayChild[1]));
						
						break;
					
					case "scale":
						arrayChild = getArray(childNode);
						if (arrayChild[0] == 0)	arrayChild[0] = 1;
						if (arrayChild[1] == 0)	arrayChild[1] = 1;
						if (arrayChild[2] == 0)	arrayChild[2] = 1;
						
//						if (_objectData is BoneData) {
//							if (yUp)
//								boneData.jointTransform.prependScale(arrayChild[0], arrayChild[1], arrayChild[2]);
//							else
//								boneData.jointTransform.prependScale(arrayChild[0], arrayChild[2], arrayChild[1]);
//						} else {
						if (yUp)
							transform.prependScale(arrayChild[0], arrayChild[1], arrayChild[2]);
						else
							transform.prependScale(arrayChild[0], arrayChild[2], arrayChild[1]);
						
						break;
					
					// Baked transform matrix
					case "matrix":
						arrayChild = getArray(childNode);
						var m:Matrix3D = new Matrix3D();
						m.rawData = array2matrix(arrayChild, yUp, _scaling);
						transform.prepend(m);
						break;
					
					case "node":
						//3dsMax 11 - Feeling ColladaMax v3.05B
						//<node><node/></node>
						parseNode(childNode, entity);
						
						break;
					
					case "instance_node":
						parseNode(collada["library_nodes"].node.(@id == getId(childNode.@url))[0], entity);
						
						break;
					
					case "instance_geometry":
						if(String(childNode).indexOf("lines") == -1) 
						{
							
							//add materials to materialLibrary
							for each (instance_material in childNode..instance_material)
								parseMaterial(instance_material.@symbol, getId(instance_material.@target));
							
							geo = collada["library_geometries"].geometry.(@id == getId(childNode.@url))[0];
							
							//meshDataList.push( new MeshData1(entity, geo.@id, geo, null) );
							//parseMesh(geo.@id, geo, null, entity);
							geometryList.push(entity);
							geometryName.push(geo.@id);
							geometryXMLList.push(geo);
							geometryControllerXMLList.push(null);
						}
						
						break;
					
					case "instance_controller":
						
						//add materials to materialLibrary
						for each (instance_material in childNode..instance_material)
							parseMaterial(instance_material.@symbol, getId(instance_material.@target));
						
						ctrlr = collada["library_controllers"].controller.(@id == getId(childNode.@url))[0];
						geo = collada["library_geometries"].geometry.(@id == getId(ctrlr["skin"][0].@source))[0];

						//meshDataList.push( new MeshData1(entity, geo.@id, geo, ctrlr) );
						geometryList.push(entity);
						geometryName.push(geo.@id);
						geometryXMLList.push(geo);
						geometryControllerXMLList.push(ctrlr);

						//(_objectData as MeshData).skeleton = getId(childNode["skeleton"]);
						
						break;
				}
			}
			
			entity.transform = transform;
			
			if(entity.bone)
				_defaultTransform[entity.name] = transform;
			
			parent.addChild(entity);
		}
		
		private function parseMesh(name:String, geometry:XML, controller:XML, parent:Entity):void
		{
			Debug.log("Parse Mesh : "+ name);
			
			var vertices:Vector.<Vertex3D> = new Vector.<Vertex3D>();
			var uv:Vector.<TextureCoordinate> = new Vector.<TextureCoordinate>();
			var faces:Vector.<Triangle3D> = new Vector.<Triangle3D>();
			
			// Triangles
			var trianglesXMLList:XMLList = geometry["mesh"].triangles;
			
			// C4D
			var isC4D:Boolean = (trianglesXMLList.length()==0 && geometry["mesh"].polylist.length()>0);
			if(isC4D)
				trianglesXMLList = geometry["mesh"].polylist;
			
			for each (var triangles:XML in trianglesXMLList)
			{
				// Input
				var field:Array = [];
				
				for each(var input:XML in triangles["input"]) 
				{
					var semantic:String = input.@semantic;
					switch(semantic) {
						case "VERTEX":
							parseVertices(input, geometry, vertices);
							break;
						case "TEXCOORD":
							parseUV(input, geometry, uv);
							break;
						default:
					}
					field.push(input.@semantic);
				}
				
				var data     :Array  = triangles["p"].split(' ');
				var len      :Number = triangles.@count;
				var symbol :String = triangles.@material;
				var material:Material = getMaterial(symbol);
				var texture:Texture = getTexture(symbol);
				
				for (var j:Number = 0; j < len; ++j)
				{
					var faceVertices:Vector.<int> = new Vector.<int>(3);
					var faceUV:Vector.<int> = new Vector.<int>(3);
					
					for (var vn:Number = 0; vn < 3; vn++)
					{
						for each (var fld:String in field)
						{
							switch(fld)
							{
								case "VERTEX":
									faceVertices[vn] = data.shift();
									break;
								case "TEXCOORD":
									faceUV[vn] = data.shift();
									break;
								default:
									data.shift();
							}
						}
					}
					
					faces.push(
						new Triangle3D(
							faceVertices[0], 
							faceVertices[2], 
							faceVertices[1],
							uv[faceUV[0]],
							uv[faceUV[2]],
							uv[faceUV[1]],
							material,
							texture,
							render_mode) );
				}
			}

			//parse controller
			if (!controller)
			{
				parent.mesh = new Mesh3D(vertices, faces);
				
				// Double Side
				if (String(geometry["extra"].technique.double_sided) != "")
					parent.mesh.doubleSide = (geometry["extra"].technique.double_sided[0].toString() == "1");

				//parent.mesh.terrainTrace = true;
				return;
			}
			
			var skin:XML = controller["skin"][0];
			
			var jointId:String = getId(skin["joints"].input.(@semantic == "JOINT")[0].@source);
			var tmp:String = skin["source"].(@id == jointId)["Name_array"].toString();
			//Blender?
			if (!tmp) 
				tmp = skin["source"].(@id == jointId)["IDREF_array"].toString();
			
			tmp = tmp.replace(/\n/g, " ");
			var nameArray:Array = tmp.split(" ");
			
			var bind_shape:Matrix3D = new Matrix3D();
			bind_shape.rawData = array2matrix(getArray(skin["bind_shape_matrix"][0].toString()), yUp, _scaling);
			
			var bindMatrixId:String = getId(skin["joints"].input.(@semantic == "INV_BIND_MATRIX").@source);
			var float_array:Vector.<Number> = getArray(skin["source"].(@id == bindMatrixId)[0].float_array.toString());
			
			var v:Array;
			var matrix:Matrix3D;
			var jointNames:Vector.<String> = new Vector.<String>();
			var bone:Entity;
			var id:String;
			var i:uint;
			var length:uint;
			var vertexTemp:Vertex3D;
			var vertex:Vector3D = new Vector3D();
			var weightsTemp:Array = new Array();
			var vertexWeights:Vector.<Vector.<Weight>> = new Vector.<Vector.<Weight>>();
			var boneOffsetMatrices:Vector.<Matrix3D> = new Vector.<Matrix3D>();
			//var bones:Vector.<Entity> = new Vector.<Entity>();
			
			length = float_array.length;
			for(i = 0; i < length; i += 16)
			{
				bone = getChild(id = nameArray[i / 16]);
				if(bone)
					jointNames.push(bone.name);
				else
					Debug.warning("no joint found for " + id);
//				bone = getChild(jointName);
//				if (bone)
//					bones.push(bone);
//				else
//					Debug.warning("no joint found for " + name);

				boneOffsetMatrices.push( new Matrix3D( array2matrix(float_array.slice(i, i+16), yUp, _scaling) ) );
			}
			
			tmp = skin["vertex_weights"][0].@count;
			var weightsId:String = getId(skin["vertex_weights"].input.(@semantic == "WEIGHT")[0].@source);
			
			tmp = skin["source"].(@id == weightsId)["float_array"].toString();
			var weights:Array = tmp.split(" ");
			
			tmp = skin["vertex_weights"].vcount.toString();
			var vCount:Array = tmp.split(" ");
			
			tmp = skin["vertex_weights"].v.toString();
			v = tmp.split(" ");
			
			var c			:int;
			var count		:int = 0;
			var skinWeights:Array;
			var weight:Weight;
			var boneIndex:int = 0;

			length = vertices.length;
			for(i = 0; i < length; i ++) 
			{
				c = int(vCount[i]);
				j=0;
				while (j < c) 
				{
					boneIndex = int(v[count]);
					count++;
					weight = new Weight(Number(weights[int(v[count])]), i);
					count++;
					++j;
					
					if(null == weightsTemp[boneIndex])
						weightsTemp[boneIndex] = new Vector.<Weight>();
					
					weightsTemp[boneIndex].push(weight);
				}
				
				vertexTemp = vertices[i];
				
				vertex.x = vertexTemp.x;
				vertex.y = vertexTemp.y;
				vertex.z = vertexTemp.z;
				
				vertex = bind_shape.transformVector(vertex);
				
				vertexTemp.x = vertex.x;
				vertexTemp.y = vertex.y;
				vertexTemp.z = vertex.z;
			}
			
			length = jointNames.length;
			for(i = 0; i < length; i ++)
				vertexWeights.push( weightsTemp[i] );
			
			parent.mesh = new Mesh3D(vertices, faces);
			
			// Double Side
			if (String(geometry["extra"].technique.double_sided) != "")
				parent.mesh.doubleSide = (geometry["extra"].technique.double_sided[0].toString() == "1");
			
			//parent.mesh.terrainTrace = true;
			
			parent.mesh.skinMeshController = new SkinMeshController(vertexWeights, vertices, boneOffsetMatrices, jointNames);
		}
		
		private function parseVertices(input:XML, geo:XML, output:Vector.<Vertex3D>):void
		{
			var id:String = input.@source.split("#")[1];
			
			// Source?
			var acc:XMLList = geo..source.(@id == id)["technique_common"].accessor;
			
			if (acc != new XMLList())
			{
				// Build source floats array
				var floId:String  = acc.@source.split("#")[1];
				var floXML:XMLList = collada..float_array.(@id == floId);
				var floStr:String  = floXML.toString();
				var floats:Vector.<Number>   = getArray(floStr);
				var float:Number;
				// Build params array
				var params:Array = [];
				var param:String;
				
				for each (var par:XML in acc["param"])
				params.push(par.@name);
				
				// Build output array
				var len:int = floats.length;
				var i:int = 0;
				var x:Number;
				var y:Number;
				var z:Number;
				while (i < len)
				{
					for each (param in params) 
					{
						float = floats[i];
						switch (param)
						{
							case "X":
								if (yUp)
									x = -float*_scaling;
								else
									x = float*_scaling;
								break;
							case "Y":
								if (yUp)
									//y = -float*_scaling;
									y = float*_scaling;
								else
									z = float*_scaling;
								break;
							case "Z":
								if (yUp)
									z = float*_scaling;
								else
									//y = -float*_scaling;
									y = float*_scaling;
								break;
							default:
						}
						++i;
					}
					output.push( new Vertex3D(x, y, z) );
				}
			}
			else
			{
				// Store indexes if no source
				var recursive :XMLList = geo..vertices.(@id == id)["input"];
				
				parseVertices(recursive[0], geo, output);
			}
		}
		
		private function parseUV(input:XML, geo:XML, output:Vector.<TextureCoordinate>):void
		{
			var id:String = input.@source.split("#")[1];
			
			// Source?
			var acc:XMLList = geo..source.(@id == id)["technique_common"].accessor;
			
			if (acc != new XMLList())
			{
				// Build source floats array
				var floId:String  = acc.@source.split("#")[1];
				var floXML:XMLList = collada..float_array.(@id == floId);
				var floStr:String  = floXML.toString();
				var floats:Vector.<Number>   = getArray(floStr);
				var float:Number;
				// Build params array
				var params:Array = [];
				var param:String;
				
				for each (var par:XML in acc["param"])
				params.push(par.@name);
				
				// Build output array
				var len:int = floats.length;
				var i:int = 0;
				var u:Number;
				var v:Number;
				while (i < len)
				{
					for each (param in params) 
					{
						float = floats[i];
						switch (param) 
						{
							case "S":
								u = float;
								break;
							case "T":
								v = 1 - float;
								break;
							default:
						}
						++i;
					}
					
					output.push( new TextureCoordinate(u, v) );
				}
			}
			else
			{
				// Store indexes if no source
				var recursive :XMLList = geo..vertices.(@id == id)["input"];
				
				parseUV(recursive[0], geo, output);
			}
		}
		
		private function parseMaterial(symbol:String, materialName:String):void
		{
			var material:Material = null;
			
			if(symbol == "FrontColorNoCulling")
				material = getMaterial(symbol);
			else
			{
				var materialData:XML = collada["library_materials"].material.(@id == materialName)[0];
				var fileName:String = null;
				
				if (materialData) 
				{
					var effectId:String = getId( materialData["instance_effect"].@url );
					var effect:XML = collada["library_effects"].effect.(@id == effectId)[0];
					
					material = getMaterial(symbol);
					material.ambient  = getColorTransform(effect..ambient[ 0]);
					material.diffuse  = getColorTransform(effect..diffuse[ 0]);
					material.specular = getColorTransform(effect..specular[0]);
					material.power    = Number(effect..shininess.float[0]);
					
					if (String(effect["extra"].technique.double_sided) != "")
						material.doubleSide = (effect["extra"].technique.double_sided[0].toString() == "1");
				}
				
				fileName = getTextureFileName(materialName);
				
				if(fileName)
					textureFileName[symbol] = fileName;
			}
		}
		
		/**
		 * Detects and parses all animation clips
		 */ 
		private function parseAnimationClips() : void
		{
			//Check for animations
			var anims:XML = collada["library_animations"][0];
			
			if (!anims) {
				Debug.warning("------------- No animations to parse -------------");
				return;
			}
			
			//Check to see if animation clips exist
			var clips:XML = collada["library_animation_clips"][0];
			
			Debug.log("------------- Begin Parse Animation -------------");
			
			var _channel_id:uint = 0;
			//loop through all animations and for each through all its channels
			var _channelName:String;
			var _channels:XMLList;
			var channelIndex:int;
			var id:String;
			var type:String;
			if(anims["animation"]["animation"].length()==0)
			{
				for each (var animation:XML in anims["animation"])
				{
					if(String(animation.@id).length>0)
						_channelName = animation.@id;
					else
					{
						// COLLADAMax NextGen;  Version: 1.1.0;  Platform: Win32;  Configuration: Release Max2009
						// issue#1 : missing channel.@id -> use automatic id instead
						Debug.warning("COLLADAMax2009 id : _"+_channel_id);
						_channelName = "_"+String(_channel_id++);
					}
					
					_channels = animation["channel"];
					if (_channels.length() == 1)
						parseChannel(animation, 0, _channelName);
					else
					{
						for (channelIndex = 0 ; channelIndex < _channels.length() ; channelIndex++)
						{
							id = _channels[channelIndex].@target;
							type = id.split("/")[1];
							
							parseChannel(animation, channelIndex, _channelName + "_subAnim_" + type);
						}
					}
				}
			}
			
			// C4D 
			// issue#1 : animation -> animation.animation
			// issue#2 : missing channel.@id -> use automatic id instead
			for each (animation in anims["animation"]["animation"])
			{
				if(String(animation.@id).length > 0)
				{
					_channelName = animation.@id;
				}else{
					Debug.warning("C4D id : _"+_channel_id);
					_channelName = "_"+String(_channel_id++);
				}
				
				_channels = animation["channel"];
				if (_channels.length() == 1)
					parseChannel(animation, 0, _channelName);
				else
				{
					for (channelIndex = 0 ; channelIndex < _channels.length() ; channelIndex++)
					{
						id = _channels[channelIndex].@target;
						type = id.split("/")[1];

						parseChannel(animation, channelIndex, _channelName + "_subAnim_" + type);
					}
				}
			}
			
			if (clips) {
				//loop through all animation clips
				for each (var clip:XML in clips["animation_clip"])
					parseAnimationClip(clip);
			}
			
			var animationClip:Vector.<SkeletalAnimation> = new Vector.<SkeletalAnimation>();
			animationClipList["default"] = animationClip;
			//create default animation clip
			_defaultAnimationClip = animationClip;
			
			for each(var channel:SkeletalAnimation in animationList)
				_defaultAnimationClip.push(channel);
			
			dispatchEvent( new LoadEvent(LoadEvent.ANIMATIONS_COMPLETE) );
			
			Debug.log("------------- End Parse Animation -------------");
		}
        
        private function parseAnimationClip(clip:XML) : void
        {
			var name:String = clip.@id;
			var animationClip:Vector.<SkeletalAnimation> = animationClipList[name] = new Vector.<SkeletalAnimation>();
			var url:String;
			
			Debug.log("parse animation clip : " + name);
			
			//TODO: Is there a need to handle case where there is multiple channels inside an animation channel (_subAnim_) ?
			for each (var channel:XML in clip["instance_animation"])
			{
				url = getId(channel.@url);
				animationClip.push(animationList[url]);
			}
        }
		
		private function parseChannel(animation:XML, channelIndex:int, channelName:String) : void
		{
			var channels:XMLList = animation["channel"];
			var channelChunk:XML = channels[channelIndex];
			var id:String = channelChunk.@target;
			var name:String = id.split("/")[0];
			
			var type:String = id.split("/")[1];
			
			if (!type)
			{
				Debug.warning("No animation type detected");
				
				return;
			}
			
			// C4D : didn't have @id, Maya 7 exporter has X/Y/Z split on translate
			if (String(animation.@id).length > 0 && (type.split(".").length == 1 || type.split(".")[1].length > 1)) 
			{
				type = type.split(".")[0];
				
				if ((type == "image" || animation.@id.split(".")[1] == "frameExtension")) 
				{
					//TODO : Material Animation
					Debug.warning("Material animation not yet implemented");
					
					return;
				}
				
			} 
			else 
				type = type.split(".").join("");
			
			//if(type == "RotX" || type == "RotY" || type == "RotZ")
			//	return;
			
			var i:int;
			var j:int;
			
			var times:Vector.<Number> = new Vector.<Number>();
			var params:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>();
			var matrices:Vector.<Matrix3D> = new Vector.<Matrix3D>();
			var interpolations:Vector.<Number> = new Vector.<Number>();
			var inTangent:Vector.<Vector.<Point>> = new Vector.<Vector.<Point>>();
			var outTangent:Vector.<Vector.<Point>> = new Vector.<Vector.<Point>>();
			
			//_defaultAnimationClip.channels[channelData.name] = channelData;
			
			Debug.log("parse channel : " + name + " : channelType : " + type);
			
			var sourceName:String = getId(channelChunk.@source);
			var sampler:XML = animation["sampler"].(@id == sourceName)[0];
			
			for each (var input:XML in sampler["input"])
			{
				var src:XML = animation["source"].(@id == getId(input.@source))[0];
				var list:Array = String(src["float_array"]).split(" ");
				var len:int = int(src["technique_common"].accessor.@count);
				var stride:int = int(src["technique_common"].accessor.@stride);
				var semantic:String = input.@semantic;
				
				//C4D : no stride defined
				if (stride == 0)
					stride=1;
				
				var p:String;
				
				switch(semantic) {
					case "INPUT":
						//此处第一帧不再播放					
						for(var v:uint = 1; v<list.length; ++v)
							times.push(Number(list[v]));
						
						break;
					case "OUTPUT":
						i=0;	//此处第一帧不再播放
						
						if (stride == 16)
						{
							var nIndex:int = 1;
							while (i < len-1)
							{			//i < len
								var m:Matrix3D = new Matrix3D();
								m.rawData = array2matrix(Vector.<Number>( list.slice(nIndex*stride, nIndex*stride + 16) ), yUp, _scaling);
								
								matrices.push(m);
								
								++i;
								++nIndex;
							}
						}
						else
						{
							while (i < len-1)
							{			//i < len
								params.push( new Vector.<Number>() );
								j = 0;
								while (j < stride) 
								{
									params[i].push(Number(list[i*stride + j]));
									++j;
								}
								++i;
							}
						}
						break;
					case "INTERPOLATION":
						for(var vn:uint = 1; vn<list.length; ++vn)
							interpolations.push(list[vn]);
						break;
					case "IN_TANGENT":
						i=0;
						while (i < len)
						{
							inTangent.push(new Vector.<Point>());
							j = 0;
							while (j < stride) {
								inTangent[i].push( new Point( Number(list[stride * i + j]), Number(list[stride * i + j + 1]) ) );
								++j;
							}
							++i;
						}
						break;
					case "OUT_TANGENT":
						i=0;
						while (i < len)
						{
							outTangent.push(new Vector.<Point>());
							j = 0;
							while (j < stride) {
								outTangent[i].push( new Point( Number(list[stride * i + j]), Number(list[stride * i + j + 1]) ) );
								++j;
							}
							++i;
						}
						break;
				}
			}
			
			var param:Vector.<Number>;
			var animationType:int;
			switch(type)
			{
				case "translateX":
				case "translationX":
				case "transform(3)(0)":
					animationType = SkeletalAnimation.TRANSLATION_X;
					if (yUp) {
						for each (param in params)
						param[0] *= -_scaling;
					} else {
						for each (param in params)
						param[0] *= _scaling;
					}
					break;
				case "translateY":
				case "translationY":
				case "transform(3)(1)":
					if (yUp)
						animationType = SkeletalAnimation.TRANSLATION_Y;
					else
						animationType = SkeletalAnimation.TRANSLATION_Z;
					
					//for each (param in params)
					//param[0] *= -_scaling;
					break;
				case "translateZ":
				case "translationZ":
				case "transform(3)(2)":
					if (yUp)
						animationType = SkeletalAnimation.TRANSLATION_Z;
					else
						animationType = SkeletalAnimation.TRANSLATION_Y;
					
					//for each (param in params)
					//param[0] *= _scaling;
					break;
				case "jointOrientX":
				case "rotateXANGLE":
				case "rotateX":
				case "RotX":
					animationType = SkeletalAnimation.ROTATION_X;
					
					//for each (param in params)
					//param[0] *= -1;
					break;
				case "jointOrientY":
				case "rotateYANGLE":
				case "rotateY":
				case "RotY":
					if (yUp)
						animationType = SkeletalAnimation.ROTATION_Y;
					else
						animationType = SkeletalAnimation.ROTATION_Z;
					
					for each (param in params)
					param[0] *= -1;
					break;
				case "jointOrientZ":
				case "rotateZANGLE":
				case "rotateZ":
				case "RotZ":
					if (yUp)
						animationType = SkeletalAnimation.ROTATION_Z;
					else
						animationType = SkeletalAnimation.ROTATION_Y;
					
					for each (param in params)
					param[0] *= -1;
					break;
				case "scaleX":
				case "transform(0)(0)":
					animationType = SkeletalAnimation.SCALE_X;
					break;
				case "scaleY":
				case "transform(1)(1)":
					if (yUp)
						animationType = SkeletalAnimation.SCALE_Y;
					else
						animationType = SkeletalAnimation.SCALE_Z;
					break;
				case "scaleZ":
				case "transform(2)(2)":
					if (yUp)
						animationType = SkeletalAnimation.SCALE_Z;
					else
						animationType = SkeletalAnimation.SCALE_Y;
					break;
				case "translate":
				case "translation":
					animationType = SkeletalAnimation.TRANSLATION;
					if (yUp) {
						for each (param in params) 
						{
							//param[0] *= -_scaling;
							//param[1] *= -_scaling;
							//param[2] *= _scaling;
							param[0] *= -_scaling;
							param[1] *=  _scaling;
							param[2] *=  _scaling;
						}
					} else {
						for each (param in params) 
						{
							//param[0] *= _scaling;
							//param[1] *= -_scaling;
							//param[2] *= _scaling;
							param[0] *= _scaling;
							param[1] *= _scaling;
							param[2] *= _scaling;
							
							param[1] = param[2] + (param[2] = param[1]) * 0;
						}
					}
					break;
				case "scale":
					animationType = SkeletalAnimation.SCALE;
					
					if(!yUp)
					{
						for each (param in params) 
						{
							param[1] = param[2] + (param[2] = param[1]) * 0;
						}
					}
					break;
				case "rotate":
					animationType = SkeletalAnimation.ROTATION;
					
					//if(yUp)
					//	for each (param in params)
					//	param[0] *= -1;
					if(!yUp)
					{
						for each (param in params) 
						param[1] = param[2] + (param[2] = param[1]) * 0;
					}
					
					for each (param in params)
					{
						param[1] *= -1;
						param[2] *= -1;
						param[3] *= -1;
					}
					
					break;
				case "transform":
					animationType = SkeletalAnimation.TRANSFORM;
					break;
				
				case "visibility":
					animationType = SkeletalAnimation.VISIBILITY;
					break;
			}
			
			var skeletalAnimation:SkeletalAnimation = new SkeletalAnimation(name, animationType, this, times, params, matrices, interpolations, inTangent, outTangent);
			animationList[channelName] = skeletalAnimation;
			
			var bone:Entity = getChild(name);
			
			if(bone)
				bone.skeletalAnimation = skeletalAnimation;
		}
		
		/**
		 * Retrieves the filename of a material
		 */
		private function getTextureFileName( materialName:String ):String
		{
			var filename :String = null;
			var material:XML = collada["library_materials"].material.(@id == materialName)[0];
	
			if( material )
			{
				var effectId:String = getId( material["instance_effect"].@url );
				var effect:XML = collada["library_effects"].effect.(@id == effectId)[0];
	
				if (effect..texture.length() == 0) return null;
	
				var textureId:String = effect..texture[0].@texture;
	
				var sampler:XML =  effect..newparam.(@sid == textureId)[0];
	
				// Blender
				var imageId:String = textureId;
	
				// Not Blender
				if( sampler )
				{
					var sourceId:String = sampler..source[0];
					var source:XML =  effect..newparam.(@sid == sourceId)[0];
	
					imageId = source..init_from[0];
				}
	
				var image:XML = collada["library_images"].image.(@id == imageId)[0];
	
				//3dsMax 11 - Feeling ColladaMax v3.05B.
				if(!image)
					filename = collada["library_images"].image.init_from.text();
				else
					filename = image["init_from"];
	
				if (filename.substr(0, 2) == "./")
				{
					filename = filename.substr( 2 );
				}
			}
			
			var baseParts:Array = basicPath ? basicPath.split("/") : new Array();
			var imgParts:Array = filename.split("/");
			
			while( baseParts[0] == "." )
				baseParts.shift();
			
			while( imgParts[0] == "." )
				imgParts.shift();
			
			while( imgParts[0] == ".." )
			{
				imgParts.shift();
				baseParts.pop();
			}
			
			filename = baseParts.length > 1 ? baseParts.join("/") : (baseParts.length?baseParts[0]:"");
			
			filename = filename != "" ? filename + "/" + imgParts.join("/") : imgParts.join("/");
			
			return filename;
		}
		
		/**
		 * Fills the 3d matrix object with values from an array with 3d matrix values
		 * ordered from right to left and up to down.
		 */
        public function array2matrix(ar:Vector.<Number>, yUp:Boolean, _scaling:Number):Vector.<Number>
        {
        	var rawData:Vector.<Number> = new Vector.<Number>(16);
        	
        	/*if (yUp) {
        		
                rawData[0] = ar[0];
                rawData[4] = ar[1];
                rawData[8] = -ar[2];
                rawData[12] = -ar[3]*_scaling;
                rawData[1] = ar[4];
                rawData[5] = ar[5];
                rawData[9] = -ar[6];
                rawData[13] = -ar[7]*_scaling;
                rawData[2] = -ar[8];
                rawData[6] = -ar[9];
                rawData[10] = ar[10];
                rawData[14] = ar[11]*_scaling;
        	} else {
        		rawData[0] = ar[0];
                rawData[4] = -ar[2];
                rawData[8] = ar[1];
                rawData[12] = ar[3]*_scaling;
                rawData[1] = -ar[8];
                rawData[5] = ar[10];
                rawData[9] = -ar[9];
                rawData[13] = -ar[11]*_scaling;
                rawData[2] = ar[4];
                rawData[6] = -ar[6];
                rawData[10] = ar[5];
                rawData[14] = ar[7]*_scaling;
        	}*/
			
			if(yUp)
			{
				rawData[0 ] = ar[0];
				rawData[4 ] = ar[1];
				rawData[8 ] = ar[2];
				rawData[12] = -ar[3]*_scaling;
				rawData[1 ] = ar[4];
				rawData[5 ] = ar[5];
				rawData[9 ] = ar[6];
				rawData[13] = ar[7]*_scaling;
				rawData[2 ] = ar[8];
				rawData[6 ] = ar[9];
				rawData[10] = ar[10];
				rawData[14] = ar[11]*_scaling;
			}
			else
			{
				rawData[0 ] = ar[0];
				rawData[4 ] = ar[2];
				rawData[8 ] = ar[1];
				rawData[12] = ar[3]*_scaling;
				rawData[1 ] = ar[8];
				rawData[5 ] = ar[10];
				rawData[9 ] = ar[9];
				rawData[13] = ar[11]*_scaling;
				rawData[2 ] = ar[4];
				rawData[6 ] = ar[6];
				rawData[10] = ar[5];
				rawData[14] = ar[7]*_scaling;
			}
				
			rawData[3 ] = ar[12];
			rawData[7 ] = ar[13];
			rawData[11] = ar[14];
			rawData[15] = ar[15];
            
            return rawData;
        }
		
		private function getColorValue(colorXML:XML):uint
		{
			if (!colorXML || colorXML.length() == 0)
				return 0xFFFFFFFF;
			
			if(!colorXML["color"] || colorXML["color"].length() == 0)
				return 0xFFFFFFFF;
			
			var colorArray:Array = colorXML["color"].split(" ");
			if(colorArray.length <= 0)
				return 0xFFFFFFFF;
			
			return 0xff000000 | int(colorArray[0]*255 << 16) | int(colorArray[1]*255 << 8) | int(colorArray[2]*255);
		}
		
		private function getColorTransform(colorXML:XML):ColorTransform
		{
			if (!colorXML || colorXML.length() == 0)
				return new ColorTransform();
			
			if(!colorXML["color"] || colorXML["color"].length() == 0)
				return new ColorTransform();
			
			var colorArray:Array = colorXML["color"].split(" ");
			if(colorArray.length <= 0)
				return new ColorTransform();
			
			return new ColorTransform(colorArray[0], colorArray[1], colorArray[2]);
		}
		
		private function getArray(spaced:String):Vector.<Number>
		{
			spaced = spaced.split("\r\n").join(" ");
			var strings:Array = spaced.split(" ");
			var numbers:Vector.<Number> = new Vector.<Number>();
			
			var totalStrings:Number = strings.length;
			
			for (var i:Number = 0; i < totalStrings; ++i)
				if (strings[i] != "")
					numbers.push(Number(strings[i]));
			
			return numbers;
		}
		
		private function getId(url:String):String
		{
			return url.split("#")[1];
		}
		
		private function getMaterial(name:String):Material
		{
			if(!materialList)
			{
				Debug.warning("no data");
				
				return null;
			}
			
			var material:Material = materialList[name];
			
			if(material)
				return material;
			
			return materialList[name] = new Material(name);
		}
		
		private function getTexture(name:String):Texture
		{
			if(!materialList)
			{
				Debug.warning("no data");
				
				return null;
			}
			
			var texture:Texture = textureList[name];
			
			if(texture)
				return texture;
			
			return textureList[name] = new Texture(name);
		}
		
		private var geometryList:Vector.<Entity>;
		private var geometryName:Vector.<String>;
		private var geometryXMLList:Vector.<XML>;
		private var geometryControllerXMLList:Vector.<XML>;
		
		private var materialList:Array;
		private var textureList:Array;
		private var textureFileName:Array;
		
		private var animationList:Array;
		private var animationClipList:Array;
		
		private var _defaultAnimationClip:Vector.<SkeletalAnimation>;
		
		private var basicPath:String;
		
		private var _loader:URLLoader;
		
		private var collada:XML;
		
		private var yUp:Boolean;
		
		private var isParseAnimation:Boolean;
		
		private var _scaling:Number;
		
		private var render_mode:uint;
    }
}