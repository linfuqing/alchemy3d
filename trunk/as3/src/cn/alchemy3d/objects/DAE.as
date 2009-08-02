package cn.alchemy3d.objects
{
	import cn.alchemy3d.animation.channel.AbstractChannel3D;
	import cn.alchemy3d.animation.channel.MatrixChannel3D;
	import cn.alchemy3d.events.LoadEvent;
	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.texture.Texture;
	import cn.alchemy3d.texture.TextureManager;
	
	import flash.events.*;
	import flash.geom.Matrix3D;
	import flash.geom.Point;
	import flash.geom.Vector3D;
	import flash.utils.ByteArray;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;
	
	import org.ascollada.ASCollada;
	import org.ascollada.core.*;
	import org.ascollada.fx.*;
	import org.ascollada.io.DaeReader;
	import org.ascollada.namespaces.*;
	import org.ascollada.types.*;
	
	/**
	 * DAE导入，你可以使用它导入外部DAE格式的3D数据.
	 * 像这样：
	 * <p>
	 * var dae:DAE = new DAE();
	 * <p>
	 * dae.load(DAE地址);
	 * <p>
	 * 如果你的DAE中包含动画.那么你可以这样获取动画播放器.
	 * <p>
	 * var controller:IController = dae.getControllerByName( "animation" );
	 */
	public class DAE extends Entity
	{
		//默认图片格式
		/** Alternative file-extension for TGA images. Default is "png". */
		public static var DEFAULT_TGA_ALTERNATIVE:String = "png";
		
		//使用材质的目标名字（材质导入时候用）
		/** Defines wether to set materials by target id or name - Might fix C4D**/
		public var useMaterialTargetName:Boolean = false;
		
		/** change this to 0 if you're DAE is picking the wrong coordinates */
		// 当发生错误时请把这个值设置为0
		public var forceCoordSet : int = 0;
		
		// collada...不知道是什么的话百度一下
		/** The loaded XML. */
		public var COLLADA:XML;
		
		//文件名：
		/** The filename - if applicable. */
		public var filename:String;
		
		//文件标题：
		/** The filetitle - if applicable. */
		public var fileTitle:String;
		
		//基础地址，DAE搜索位图时用
		/** Base url. */
		public var baseUrl:String;
		
		//图片地址：
		/** Path where the textures should be loaded from. */
		public var texturePath:String;
		
		//DAE导入器:
		/** The COLLADA parser. */
		public var parser:DaeReader;
		
		//DAE库:
		/** The DaeDocument. @see org.ascollada.core.DaeDocument */
		public var document:DaeDocument;
		
		//DAEID:	
		/** */
		private var _colladaID:Dictionary;
		
		//DAE备用ID
		/** */
		private var _colladaSID:Dictionary;
		
		//存储DAEID项目：
		/** */
		private var _colladaIDToObject:Object;
		
		//存储DAESID项目：
		/** */
		private var _colladaSIDToObject:Object;
		
		//结点项目：
		/** */
		private var _objectToNode:Object;
		
		//存储动画控制器（下标为目标）：
		/** */
		private var _channelsByTarget:Dictionary;
		
		//几何体列表：
		/** */
		private var _geomVertices:Object;
		private var _geomFaces:Object;
		
		//渲染材质列表：
		/** */
		private var _queuedMaterials:Array;
		
		//图片设置列表：
		/** */
		private var _textureSets:Object;
		
		//骨骼列表：
		/** */
		private var _skins:Dictionary;
		
		//骨骼数：
		/** */
		private var _numSkins:uint;
		
		//根结点:
		/** */
		public var _rootNode:Entity;
		
		//是否动画：
		/** */
		private var _isAnimated:Boolean = false;
		
		//数组动画列表：
		private var _bindVertexInputs : Dictionary;
		
		/** */
		protected var _channels:Vector.<AbstractChannel3D>;
		
		/** */
		protected var _currentFrame:int = 0;
		
		/** */
		protected var _currentTime:int;
		
		/** */
		protected var _totalFrames:int = 0;
		
		/** */
		protected var _startTime:Number;
		
		/** */
		protected var _endTime:Number;
		
		/** */
		protected var _autoPlay:Boolean = true;
		
		protected var _loop:Boolean = false;
		
		/** */
		protected var _isPlaying:Boolean = false;
		
		public var materials:TextureManager;
		
		public var _allVertices:Vector.<Vertex3D>;
		public var _allFaces:Vector.<Triangle3D>;
		
		/**
		 * 构造函数.
		 * constructor.
		 * 
		 */
		public function DAE(autoPlay:Boolean=true, loop:Boolean=false, name:String=null)
		{
			super(null, null, name);
			
			this._autoPlay = autoPlay;
			this._loop = loop;
			
			this.materials = new TextureManager();
			_allVertices = new Vector.<Vertex3D>();
			_allFaces = new Vector.<Triangle3D>();
		}
				
		/**
		 * DAE读取
		 * 
		 * Loads the COLLADA.
		 * 
		 * @param	asset The url, an XML object or a ByteArray specifying the COLLADA file.
		 * @param	materials	An optional materialsList.
		 */ 
		public function load(asset:*, asynchronousParsing : Boolean = false):void
		{
			this.materials = new TextureManager();
			
			buildFileInfo(asset);
			
			_bindVertexInputs = new Dictionary(true);
			
			this.parser = new DaeReader(asynchronousParsing);
			this.parser.addEventListener(Event.COMPLETE, onParseComplete);
			this.parser.addEventListener(ProgressEvent.PROGRESS, onParseProgress);
			
			if(asset is XML)
			{
				this.COLLADA = asset as XML;
				this.parser.loadDocument(asset);
			}
			else if(asset is ByteArray)
			{
				this.COLLADA = new XML(ByteArray(asset));
				this.parser.loadDocument(asset);
			}
			else if(asset is String)
			{
				this.filename = String(asset);
				this.parser.read(this.filename);
			}
			else
			{
				throw new Error("load : unknown asset type!");
			}
		}
		
		/**
		 * 建造文件地址
		 * 
		 * @param	asset
		 * @return
		 */
		private function buildFileInfo( asset:* ):void
		{
			this.filename = asset is String ? String(asset) : "./";
			
			// make sure we've got forward slashes!
			this.filename = this.filename.split("\\").join("/");
				
			if( this.filename.indexOf("/") != -1 )
			{
				// dae is located in a sub-directory of the swf.
				var parts:Array = this.filename.split("/");
				this.fileTitle = String( parts.pop() );
				this.baseUrl = parts.join("/");
			}
			else
			{
				// dae is located in root directory of swf.
				this.fileTitle = this.filename;
				this.baseUrl = "";
			}
		}

		/**
		 * DAE读取完成调用
		 * 
		 * Called when the DaeReader completed parsing.
		 * 
		 * @param	event
		 */
		private function onParseComplete(event:Event):void
		{
			dispatchEvent(new LoadEvent(LoadEvent.DOCUMENT_LOAD_COMPLETE));
			
			var reader:DaeReader = event.target as DaeReader;
			
			this.document = reader.document;
			
			_textureSets = new Object();
			_colladaID = new Dictionary(true);
			_colladaSID = new Dictionary(true);
			_colladaIDToObject = new Object();
			_colladaSIDToObject = new Object();
			_objectToNode = new Object();
			_skins = new Dictionary(true);
			
			buildTextures();
			loadNextTexture();
		}
		
		/**
		 * Called on parsing progress.
		 * 
		 * @param	event
		 */ 
		protected function onParseProgress(e:ProgressEvent):void
		{
			dispatchEvent(new LoadEvent(LoadEvent.PROGRESS, this.filename, e.bytesLoaded, e.bytesTotal));
		}
		
		/**
		 * 建立材质
		 * 
		 * Builds the materials.
		 */ 
		private function buildTextures():void
		{
			_queuedMaterials = [];
			
			for( var materialId:String in this.document.materials )
			{
				var daeMaterial:DaeMaterial = this.document.materials[ materialId ];
				
				var symbol:String = this.document.materialTargetToSymbol[ daeMaterial.id ];
				
				var texture:Texture;
				if(useMaterialTargetName)
					texture = this.materials.getMaterialByName(materialId);
				else
					texture = this.materials.getMaterialByName(symbol);
				
				if( texture == null) 
				{
				    var tID:String = getSymbolName( daeMaterial.effect );
				    texture = this.materials.getMaterialByName(tID);
				    // remove bad reference BEFORE adding the next - otherwise, they'll both be removed
				    materials.removeMaterial(texture);
				    
				    if( texture ) 
				    {
				        if(useMaterialTargetName)
				             this.materials.addMaterial(texture, materialId);
				        else
				             this.materials.addMaterial(texture, symbol);
				    }
				}
					
				var effect:DaeEffect = document.effects[ daeMaterial.effect ];
				
				var lambert:DaeLambert = effect.color as DaeLambert;
				
				// save the texture-set if necessary
				if(lambert && lambert.diffuse.texture)
					_textureSets[symbol] = lambert.diffuse.texture.texcoord;
				
				// Texture already exists in our materialsList, no need to process
				if(texture) continue;
				
				// if the Texture has a texture, qeueu the bitmap
				if(effect && effect.texture_url)
				{				
					var image:DaeImage = document.images[effect.texture_url];
					if(image)
					{
						var imageUrl:String = buildImagePath(this.baseUrl, image.init_from);
						
						texture = new Texture();
						
						_queuedMaterials.push({symbol:symbol, url:imageUrl, texture:Texture, target:materialId});
						
						continue;
					}
				}

				if(lambert && lambert.diffuse.color)
				{
					if(effect.wireframe)
					{
//						Texture = new WireframeMaterial(1, buildColor(lambert.diffuse.color), 1);
					}
					else
						texture = null;
				}
				else
					texture = null;
					
				texture.doubleSide = effect.double_sided;
				
				if(useMaterialTargetName)
					this.materials.addMaterial(texture, materialId);
				else
					this.materials.addMaterial(texture, symbol);
			}
		}
		
		/**
		 * 从名字获取名称
		 */	
		private function getSymbolName(effectID:String):String
		{
		    var ef:DaeEffect;
		    var effect:DaeEffect;
		    for each ( effect in this.document.effects )
		    {
		       // trace("locating effect", effect.id, effectID);
		        if( effect.id == effectID ) 
		        {
		            return effect.texture_url;
		        }
		    }

		    return null;
		}
		
		/**
		 *建立位图地址
		 * 
		 * @return
		 */
		private function buildImagePath( meshFolderPath:String, imgPath:String ):String
		{
			if (texturePath != null)
				imgPath = texturePath + imgPath.slice( imgPath.lastIndexOf("/") + 1 );
			
			var baseParts:Array = meshFolderPath.split("/");
			var imgParts:Array = imgPath.split("/");
			
			while( baseParts[0] == "." )
				baseParts.shift();
				
			while( imgParts[0] == "." )
				imgParts.shift();
				
			while( imgParts[0] == ".." )
			{
				imgParts.shift();
				baseParts.pop();
			}
						
			var imgUrl:String = baseParts.length > 1 ? baseParts.join("/") : (baseParts.length?baseParts[0]:"");
						
			imgUrl = imgUrl != "" ? imgUrl + "/" + imgParts.join("/") : imgParts.join("/");
			
			return imgUrl;
		}
		
		/**
		 * 读取外部材质
		 * 
		 * Loads the next Texture.
		 * 
		 * @param	event
		 */ 
		private function loadNextTexture(event:LoadEvent = null):void
		{
			if(event)
			{
				var previous:Texture = event.target as Texture;
				previous.removeEventListener(LoadEvent.TEXTURE_LOAD_COMPLETE, loadNextTexture);
				previous.removeEventListener(LoadEvent.PROGRESS, onProgress);
				previous.removeEventListener(LoadEvent.LOAD_ERROR, onMaterialError);
			}
			
			if(_queuedMaterials.length)
			{
				var data:Object = _queuedMaterials.shift();
				var url:String = data.url;
				var symbol:String = data.symbol;
				var target:String = data.target;
				
				url = url.replace(/\.tga/i, "." + DEFAULT_TGA_ALTERNATIVE);
						
				var texture:Texture = data.texture;
				texture.name = symbol;
				texture.addEventListener(LoadEvent.TEXTURE_LOAD_COMPLETE, loadNextTexture);
				texture.addEventListener(LoadEvent.PROGRESS, onProgress);
				texture.addEventListener(LoadEvent.LOAD_ERROR, onMaterialError);
				
				// shouldn't need the ?nc no cache filename now that BitmapFileMaterial has been fixed. 
				texture.load(url);

				if(useMaterialTargetName)
				{
					texture.name = target;
					this.materials.addMaterial(texture, target);
				}
				else
				{
					texture.name = symbol;
					this.materials.addMaterial(texture, symbol);
				}
			}
			else
			{
				dispatchEvent(new LoadEvent(LoadEvent.COLLADA_MATERIALS_LOAD_COMPLETE));
				
				onMaterialsLoaded();
			}
		}
		
		private function onMaterialsLoaded() : void
		{
			if(this.parser.hasEventListener(Event.COMPLETE))
				this.parser.removeEventListener(Event.COMPLETE, onParseComplete);
			if(this.parser.hasEventListener(ProgressEvent.PROGRESS))
				this.parser.removeEventListener(ProgressEvent.PROGRESS, onParseProgress);

			//may have geometries to be parsed
			if(document.numQueuedGeometries)
			{
				this.parser.addEventListener(Event.COMPLETE, onParseGeometriesComplete);
				//this.parser.addEventListener(ProgressEvent.PROGRESS, onParseGeometriesProgress);
				this.parser.readGeometries();
			}
			else
			{
				buildScene(); 
			}
		}
		
		private function onParseGeometriesComplete(e:Event) : void
		{
			if(this.parser.hasEventListener(Event.COMPLETE))
				this.parser.removeEventListener(Event.COMPLETE, onParseGeometriesComplete);
			buildScene(); 
		}
		
		/**
		 * 建立场景
		 * 
		 * Builds the scene.
		 */ 
		private function buildScene():void
		{
			buildGeometries();
			
			_rootNode = new Entity(null, null, "COLLADA_Scene");
			
			for(var i:int = 0; i < this.document.vscene.nodes.length; i++)
			{
				buildNode(this.document.vscene.nodes[i], _rootNode);
			}
			
			// link the skins
			//TODO
//			linkSkins();
			
			this.addChild(_rootNode);
			
			_rootNode.rotationX = -90;
			
			dispatchEvent(new LoadEvent(LoadEvent.GEOMETRY_BUILD_COMPLETE));

//			// animation stuff
//			_currentFrame = 0;
//			_totalFrames = 0;
//			_startTime = _endTime = 0;
//			_channels = new Vector.<AbstractChannel3D>;
//			_isAnimated = false;
//			_isPlaying = false;
//			
//			//读取动画
//			if(document.numQueuedAnimations)
//			{
//				_isAnimated = true;
//				
//				this.parser.addEventListener(Event.COMPLETE, onParseAnimationsComplete);
//				this.parser.addEventListener(ProgressEvent.PROGRESS, onParseAnimationsProgress);
//				this.parser.readAnimations();
//			}
//			//场景建立完毕
//			//dispatchEvent(new LoadEvent(LoadEvent.LOAD_COMPLETE));
		}

		/**
		 * 建立几何体
		 * 
		 * Builds all COLLADA geometries.
		 */ 
		private function buildGeometries():void
		{
			var i:int, j:int, k:int;
			
			_geomVertices = new Object();
			_geomFaces = new Object();

			var geometry:DaeGeometry;
			for each(geometry in this.document.geometries)
			{
				if(geometry.mesh)
				{
					var vertices:Vector.<Vertex3D>;
					var faces:Vector.<Triangle3D>;
					
					//建立顶点
					for(i = 0; i < geometry.mesh.vertices.length; i++ )
					{
						buildVertices(geometry.mesh.vertices[i], vertices);
					}

					if(!vertices.length)
						continue;
					
					//建立面
					for(i = 0; i < geometry.mesh.primitives.length; i++)
					{
						buildFaces(geometry.mesh.primitives[i], vertices, faces, 0);
					}
					
					_geomVertices[geometry.id] = vertices;
					_geomFaces[geometry.id] = faces;
				}
				else if(geometry.spline && geometry.splines)
				{
//					var lines:Lines3D = new Lines3D(Texture.DEFAULT_LINE, geometry.id);
//					
//					for(i = 0; i < geometry.splines.length; i++)
//					{
//						var spline:DaeSpline = geometry.splines[i];
//						
//						for(j = 0; j < spline.vertices.length; j++)
//						{
//							k = (j+1) % spline.vertices.length;
//							
//							lines.addLine(1, spline.vertices[j][0], spline.vertices[j][1], spline.vertices[j][2], spline.vertices[k][0], spline.vertices[k][1], spline.vertices[k][2]);
//						}
//					}
//					
//					_geometries[geometry.id] = lines;
				}
			}
		}
		
		/**
		 * 建立顶点
		 */	
		private function buildVertices(gvertices:Array, vertices:Vector.<Vertex3D>):void
		{
			var v:Vertex3D = new Vertex3D(gvertices[0], gvertices[1], gvertices[2], 1);
			vertices.push(v);
		}
		
		/**
		 * 建立面
		 */
		private function buildFaces(primitive:DaePrimitive, vertices:Vector.<Vertex3D>, faces:Vector.<Triangle3D>, voffset:uint):void
		{
//			var faces:Array = [];
			var texture:Texture = this.materials.getMaterialByName(primitive.material);
			
			texture = texture || null;
			
			var instanceMaterial : DaeInstanceMaterial = _bindVertexInputs[ texture ];
			
			// retreive correct texcoord-set for the Texture.
			var setID:int = this.forceCoordSet;
			var textureSet:String = _textureSets[primitive.material];
			var geom:DaeGeometry = primitive.mesh.geometry;
			
			if( textureSet && textureSet.length && geom )
			{
				var instGeom:DaeInstanceGeometry = this.document.getDaeInstanceGeometry( geom.id );
				if( instGeom )
				{
					var vinput:DaeBindVertexInput = instGeom.findBindVertexInput( primitive.material, textureSet );
					if( vinput )
					{
						trace( "using input set #" + vinput.input_set + " for Texture " + primitive.material );
						setID = vinput.input_set;
					}
				}
			}
			
			var texCoordSet:Array = primitive.getTexCoords(setID); 
			var texcoords:Array = [];
			var i:int, j:int = 0, k:int;

			// texture coords
			for( i = 0; i < texCoordSet.length; i++ ) 
				texcoords.push(new Point(texCoordSet[i][0], 1 - texCoordSet[i][1]));
			
			var hasUV:Boolean = (texcoords.length == primitive.vertices.length);

			var idx:Array = [];
			var v:Array = [];
			var uv:Array = [];
			
			var tri:Triangle3D;
			switch( primitive.type ) 
			{
				// Each line described by the mesh has two vertices. The first line is formed 
				// from first and second vertices. The second line is formed from the third and fourth 
				// vertices and so on.
				case ASCollada.DAE_LINES_ELEMENT:
					for( i = 0; i < primitive.vertices.length; i += 2 ) 
					{
						v[0] = vertices[ primitive.vertices[i] ];
						v[1] = vertices[ primitive.vertices[i+1] ];
						uv[0] = hasUV ? texcoords[  i  ] : new Point();
						uv[1] = hasUV ? texcoords[ i+1 ] : new Point();
						//geometry.faces.push( new Triangle3D(instance, [v[0], v[1], v[1]], Texture, [uv[0], uv[1], uv[1]]) );
					}
					break;
					
				// simple triangles
				case ASCollada.DAE_TRIANGLES_ELEMENT:
					for(i = 0, j = 0; i < primitive.vertices.length; i += 3, j++) 
					{
						idx[0] = voffset + primitive.vertices[i];
						idx[1] = voffset + primitive.vertices[i+1];
						idx[2] = voffset + primitive.vertices[i+2];
						
						v[0] = vertices[ idx[0] ];
						v[1] = vertices[ idx[1] ];
						v[2] = vertices[ idx[2] ];
						
						uv[0] = hasUV ? texcoords[ i+0 ] : new Point();
						uv[1] = hasUV ? texcoords[ i+1 ] : new Point();
						uv[2] = hasUV ? texcoords[ i+2 ] : new Point();
						
						tri = new Triangle3D(idx[0], idx[1], idx[2], uv[2], uv[1], uv[0], this, "triangle" + i); 
						faces.push(tri);
						//_allGeometrys.faces.push(tri);
					}
					break;
				// polygon with *no* holes
				case ASCollada.DAE_POLYLIST_ELEMENT:
					for( i = 0, k = 0; i < primitive.vcount.length; i++ ) 
					{
						var poly:Array = [];
						var uvs:Array = [];
						
						for( j = 0; j < primitive.vcount[i]; j++ ) 
						{
							uvs.push( (hasUV ? texcoords[ k ] : new Point()) );
							poly.push( vertices[primitive.vertices[k++]] );
						}
						
						if( !faces || !vertices )
							throw new Error( "no Geometry3D" );
							
						v[0] = poly[0];
						uv[0] = uvs[0];

						for( j = 1; j < poly.length - 1; j++ )
						{
							v[1] = poly[j];
							v[2] = poly[j+1];
							uv[1] = uvs[j];
							uv[2] = uvs[j+1];
							
							if( v[0] is Vertex3D && v[1] is Vertex3D && v[2] is Vertex3D )
							{
								tri = new Triangle3D(v[2], v[1], v[0], uv[2], uv[1], uv[0], this, "triangle" + i); 
								faces.push(tri);
								//_allGeometrys.faces.push(tri);
							}
							else
							{
								trace("" +primitive.name+ " "+ poly.length +" "+primitive.vertices.length+" " + vertices.length);
							}
						}
					}
					break;
				
				// polygons *with* holes (but holes not yet processed...)
				case ASCollada.DAE_POLYGONS_ELEMENT:
					for(i = 0, k = 0; i < primitive.polygons.length; i++)
					{
						var p:Array = primitive.polygons[i];
						var np:Array = [];
						var nuv:Array = [];
						
						for(j = 0; j < p.length; j++)
						{
							nuv.push( (hasUV ? texcoords[ k ] : new Point()) );
							np.push( vertices[primitive.vertices[k++]] );
						}
						
						v[0] = np[0];
						uv[0] = nuv[0];
						
						for(j = 1; j < np.length - 1; j++)
						{
							v[1] = np[j];
							v[2] = np[j+1];
							uv[1] = nuv[j];
							uv[2] = nuv[j+1];
							
							tri = new Triangle3D(v[2], v[1], v[0], uv[2], uv[1], uv[0], this, "triangle" + i); 
							faces.push(tri);
							//_allGeometrys.faces.push(tri);
						}
					}
					break;
						
				default:
					throw new Error("Don't know how to create face for a DaePrimitive with type = " + primitive.type);
			}
			trace("faces" + faces.length);
		}
		
		/**
		 * 建立结点
		 * 
		 * Builds a Mesh3D from a node. @see org.ascollada.core.DaeNode
		 * 
		 * @param	node	
		 * 
		 * @return	The created Mesh3D. @see org.papervision3d.objects.Mesh3D
		 */ 
		private function buildNode(node:DaeNode, parent:Entity):void
		{
			var instance:Entity;
			var texture:Texture;
			var i:int;
			
			if(node.controllers.length)
			{
				// controllers, can be of type 'skin' or 'morph'
				for(i = 0; i < node.controllers.length; i++)
				{
					var instanceController:DaeInstanceController = node.controllers[i];
					var colladaController:DaeController = document.controllers[instanceController.url];

					if(colladaController.skin)
					{
						instance = new Skin3D(null, null, node.name);
						
						buildSkin(instance as Skin3D, colladaController.skin, instanceController.skeletons, node);
					}
					else if(colladaController.morph)
					{
						throw new Error("morph!");
					}
					else
						throw new Error("A COLLADA controller should be of type <skin> or <morph>!");
				
					// dunnu yet how to handle multiple controllers.
					break;
				}
			}
			else if(node.geometries.length)
			{
				// got Geometry3D, so create a Mesh3D
				instance = new Mesh3D(null, null, node.name)
				var triangleMesh:Mesh3D = instance as Mesh3D;
				
				// add all COLLADA geometries to the Mesh3D
				var geom:DaeInstanceGeometry;
				for each(geom in node.geometries)
				{
					//if it's line3d
					if(_geometries[ geom.url ] is Lines3D)
					{
						triangleMesh.addChild(_geometries[ geom.url ]);
						continue;
					}
					
					var geometry:Geometry3D = _geometries[ geom.url ];		
					if(!_geomFaces || !_geomVertices)
						continue;
					
					var materialInstances:Array = [];
					if(geom.materials)
					{
						var instanceMaterial:DaeInstanceMaterial;
						for each(instanceMaterial in geom.materials)
						{
							if(useMaterialTargetName)
								texture = this.materials.getMaterialByName(instanceMaterial.target);
							else
								texture = this.materials.getMaterialByName(instanceMaterial.symbol);
							
							if(texture)
							{
								_bindVertexInputs[ texture ] = instanceMaterial;
								materialInstances.push(texture);
							}
						}
					}
					
					mergeGeometries(triangleMesh.geometry, geometry.clone(triangleMesh), materialInstances);
				}
			}
			else
			{
				// no Geometry3D, simply create a Joint3D
				instance = new Entity(null, null, node.name);
			}
			
			// recurse node instances
			for(i = 0; i < node.instance_nodes.length; i++)
			{
				var dae_node:DaeNode = document.getDaeNodeById(node.instance_nodes[i].url);
				buildNode(dae_node, instance);
			}
			
			// setup the initial transform
			var m:Matrix3D = buildMatrix(node);
			instance.copyTransformFromMatrix(m);
			
			// recurse node children
			for(i = 0; i < node.nodes.length; i++)
				buildNode(node.nodes[i], instance);
					
			// save COLLADA id, sid
			_colladaID[instance] = node.id;
			_colladaSID[instance] = node.sid;
			_colladaIDToObject[node.id] = instance;
			_colladaSIDToObject[node.sid] = instance;
			_objectToNode[instance] = node;
			
			parent.addChild(instance);
		}
		
		/**
		 * 建立结点变换数组
		 * 
		 * Builds a Matrix3D from a node's transform array. @see org.ascollada.core.DaeNode#transforms
		 * 
		 * @param	node
		 * 
		 * @return
		 */
		private function buildMatrix(node:DaeNode):Matrix3D 
		{
			var stack:Array = buildMatrixStack(node);
			var matrix:Matrix3D = new Matrix3D();
			for( var i:int = 0; i < stack.length; i++ ) 
				matrix.prepend(stack[i]);
			return matrix;
		}
		
//		/**
//		 * 建立结点变换数组集
//		 * 
//		 * @param	node
//		 * @return
//		 */
//		private function buildMatrixStack(node:DaeNode):Array
//		{
//			var stack:Array = [];	
//			for( var i:int = 0; i < node.transforms.length; i++ ) 
//				stack.push(buildMatrixFromTransform(node.transforms[i]));
//			return stack;
//		}
//		
//				
//		/**
//		 * 从变换提取数组
//		 * 
//		 * @param	node
//		 * @return
//		 */
//		private function buildMatrixFromTransform(transform:DaeTransform):Matrix3D
//		{
//			var matrix:Matrix3D = new Matrix3D();
//			var v:Array = transform.values;
//			
//			switch(transform.type)
//			{
//				case ASCollada.DAE_ROTATE_ELEMENT:
//					matrix.appendRotation( v[3],new Vector3D( v[0], v[1], v[2] ));
//					break;
//				case ASCollada.DAE_SCALE_ELEMENT:
//					matrix.appendScale(v[0], v[1], v[2]);
//					break;
//				case ASCollada.DAE_TRANSLATE_ELEMENT:
//					matrix.appendTranslation(v[0], v[1], v[2]);
//					break;
//				case ASCollada.DAE_MATRIX_ELEMENT:
//					var rawData:Vector.<Number> = Vector.<Number>(v);
//					matrix = new Matrix3D(rawData);
//					matrix.transpose();
//					break;
//				default:
//					throw new Error("Unknown transform type: " + transform.type);
//			}
//			return matrix;
//		}
//		
//		/**
//		 * 建立骨骼
//		 * 
//		 * Builds a skin.
//		 * 
//		 * @param	instance
//		 * @param	colladaSkin
//		 * @param	skeletons
//		 */ 
//		private function buildSkin(instance:Skin3D, colladaSkin:DaeSkin, skeletons:Array, node:DaeNode):void
//		{
//			var geom:Geometry3D = _geometries[ colladaSkin.source ];
//			if(!geom)
//			{
//				// geometry can be inside a morph controller
//				var morphController:DaeController = this.document.controllers[colladaSkin.source];
//				if(morphController && morphController.morph)
//				{
//					var morph:DaeMorph = morphController.morph;
//					
//					// fetch Geometry3D
//					geom = _geometries[morph.source];
//
//					// fetch target geometries
//					for(var j:int = 0; j < morph.targets.length; j++)
//					{
//						var targetGeometry:Geometry3D = _geometries[morph.targets[j]];
//					}
//				}
//				if(!geom)
//					throw new Error("no geometry for source: " + colladaSkin.source);
//			}
//			
//			mergeGeometries(instance.geometry, geom.clone(instance));
//			
//			_skins[ instance ] = colladaSkin;
//		}
//		
//				
//		/**
//		 * 合并几何体
//		 * 
//		 * Merge geometries.
//		 * 
//		 * @param target The target Geometry3D to merge to.
//		 * @param source The source Geometry3D
//		 * @param Texture Optional Texture for triangles, only used when a triangle has no Texture.
//		 */ 
////		private function mergeGeometries(targetGeom:Geometry3D, sourceGeom:Geometry3D, materialInstances:Array=null):void
//		private function mergeGeometries(targetV:Vector.<Vertex3D>, targetF:Vector.<Triangle3D>, sourceV:Vector.<Vertex3D>, sourceF:Vector.<Triangle3D>, materialInstances:Array=null):void
//		{
//			if(materialInstances && materialInstances.length)
//			{
//				var firstMaterial:Texture = materialInstances[0];
//				var texture:Texture;
//				var triangle:Triangle3D;
//				for each(triangle in sourceF)
//				{
//					var correctMaterial:Boolean = false;
//					for each (texture in materialInstances)
//					{
//						if(texture === triangle.texture)
//						{
//							correctMaterial = true;
//							break;
//						}
//					}
//					triangle.texture = correctMaterial ? triangle.texture : firstMaterial;
//				}
//			}
//			targetV = targetV.concat(sourceV);
//			targetF = targetF.concat(sourceF);
//			
//			_allVertices = _allVertices.concat(sourceV);
//			_allFaces = _allFaces.concat(sourceF);
//		}
//				
//		/**
//		 * 安装骨骼控制器.
//		 * Setup the skin controllers.
//		 */ 
//		private function linkSkins():void
//		{
//			_numSkins = 0;
//			
//			for(var object:* in _skins)
//			{
//				var instance:Skin3D = object as Skin3D;
//				
//				if(!instance)
//					throw new Error("Not a Skin3D?");
//				
//				//确保skin在所有bones最后，以便更新顶点
//				var index:int = _rootNode.children.indexOf(instance);
//				if(index > -1)
//				{
//					_rootNode.children.splice(index, 1);
//					_rootNode.children.push(instance);
//				}
//				else
//					throw new Error("RootNode do not have this Skin3D?");
//				
//				linkSkin(instance, _skins[object]);
//				_numSkins++;
//			}
//		}
//		
//		/**
//		 * 安装骨骼控制器.
//		 * Setup the skin controllers.
//		 */ 
//		protected function linkSkin(instance:Skin3D, skin:DaeSkin):void
//		{			
//			var i:int;
//			var found:Object = new Object();
//			
//			var bindShapeMatrix:Matrix3D = new Matrix3D(Vector.<Number>(skin.bind_shape_matrix));
//			bindShapeMatrix.transpose();
//			
//			var joints:Array = [];
//			var invBindMatrices:Array = [];
//			var vertexWeights:Array = [];
//			
//			for(i = 0; i < skin.joints.length; i++)
//			{
//				var jointId:String = skin.joints[i];
//				
//				if(found[jointId])
//					continue;
//					
//				var joint:Entity = _colladaIDToObject[jointId];
//				if(!joint)
//					joint = _colladaSIDToObject[jointId];
//				if(!joint)
//					throw new Error("Couldn't find the joint id = " + jointId);
//
//				var vertexWeight:Array = skin.findJointVertexWeightsByIDOrSID(jointId);
//				if(!vertexWeight)
//					throw new Error("Could not find vertex weights for joint with id = " + jointId);
//					
//				var bindMatrix:Array = skin.findJointBindMatrix2(jointId);
//				if(!bindMatrix || bindMatrix.length != 16)
//					throw new Error("Could not find inverse bind matrix for joint with id = " + jointId);
//				
//				var invBindMatrice:Matrix3D = new Matrix3D(Vector.<Number>(bindMatrix));
//				invBindMatrice.transpose();
//				invBindMatrices.push(invBindMatrice);
//				joints.push(joint);
//				vertexWeights.push(vertexWeight);
//				
//				found[jointId] = true;
//			}
//			
//			instance.setJointsData(bindShapeMatrix, joints, vertexWeights, invBindMatrices);
//		}
//		
		/**
		 * Called when a Texture failed to load.
		 * 
		 * @param	event
		 */ 
		private function onProgress(e:LoadEvent):void
		{
			dispatchEvent(new LoadEvent(LoadEvent.PROGRESS, e.filename, e.bytesLoaded, e.bytesTotal));
		}
		
		/**
		 * Called when a Texture failed to load.
		 * 
		 * @param	event
		 */ 
		private function onMaterialError(e:LoadEvent):void
		{
			loadNextTexture();	
			trace("load Texture error!");
		}
//			
//		/**
//		 * 动画读取完成时调用
//		 * 
//		 * Called when the parser completed parsing animations.
//		 * 
//		 * @param	event
//		 */ 
//		private function onParseAnimationsComplete(event:Event):void
//		{	
//			buildAnimationChannels();
//					
//			_channels = this.getAnimationChannels() || new Vector.<AbstractChannel3D>;	
//			_currentFrame = _totalFrames = 0;
//			_startTime = _endTime = 0;
//			
//			var channel:AbstractChannel3D;
//			for each(channel in _channels)
//			{
//				_totalFrames = Math.max(_totalFrames, channel.keyFrames.length);	
//				_startTime = Math.min(_startTime, channel.startTime);
//				_endTime = Math.max(_endTime, channel.endTime);
//				channel.updateToTime(0);
//			}
//			
//			trace( "animations COMPLETE (#channels: " + _channels.length + " #frames: " + _totalFrames + ", startTime: " + _startTime + " endTime: " + _endTime+ ")");
//			
//			dispatchEvent(new LoadEvent(LoadEvent.ANIMATIONS_COMPLETE));
//			
//			if(_autoPlay)
//				play();
//		}
//		
//		/**
//		 * Plays the animation.
//		 * 
//		 * @param 	clip	Optional clip name.
//		 */ 
//		public function play(clip:String=null):void
//		{
//			_currentFrame = 0;
//			_currentTime = getTimer();
//			_isPlaying = (_isAnimated && _channels && _channels.length);
//		}
//		
//		/**
//		 * 建立动画控制器
//		 * 
//		 * Build all animation channels.
//		 */ 
//		private function buildAnimationChannels():void
//		{
//			var target:Entity;
//			var channel:DaeChannel;
//			var channelsByObject:Dictionary = new Dictionary(true);
//			var i:int;
//			
//			_channelsByTarget = new Dictionary(true);
//			
//			var animation:DaeAnimation
//			for each(animation in this.document.animations)
//			{
//				for(i = 0; i < animation.channels.length; i++)
//				{
//					channel = animation.channels[i];
//					
//					target = _colladaIDToObject[channel.syntax.targetID];
//					if(!target)
//						throw new Error("damn");
//						
//					if(!channelsByObject[target])
//						channelsByObject[target] = new Array();
//					
//					channelsByObject[target].push(channel);
//				}
//			}
//			
//			for(var object:* in channelsByObject)
//			{
//				target = object as Entity;
//							
//				var channels:Array = channelsByObject[object];
//				var node:DaeNode = _objectToNode[target];
//					
//				if(!node)
//					throw new Error("Couldn't find the targeted object with name '" + node.name + "'");
//					
//				node.channels = channels;
//				
//				if(!channels.length)
//					continue;
//				
//				channel = channels[0];
//				
//				var transform:DaeTransform = node.findMatrixBySID(channel.syntax.targetSID);
//				
//				if(!transform)
//				{
//					trace("Could not find a transform with SID=" + channel.syntax.targetSID);
//					continue;
//				}
//	
//				// the object has a single <matrix> channel
//				if(channels.length == 1 && transform.type == ASCollada.DAE_MATRIX_ELEMENT)
//				{
//					_channelsByTarget[target] = buildAnimationChannel(target, channel);
//					continue;
//				}
//				
//				// the object has multiple channels, lets bake 'm into a single channel
//				var allTimes:Array = new Array();
//				var times:Array = new Array();
//				var lastTime:Number;
//				
//				// fetch all times for all channels
//				for each(channel in channels)
//					allTimes = allTimes.concat(channel.input);
//				allTimes.sort(Array.NUMERIC);
//				
//				// make array with unique times
//				for(i = 0; i < allTimes.length; i++)
//				{
//					var t:Number = allTimes[i];
//					if(i == 0)
//						times.push(t);
//					else if(t - lastTime > 0.01)
//						times.push(t);
//					lastTime = t;	
//				}
//				
//				// build the MatrixChannel3D's for this object
//				var mcs:Object = new Object();
//				for each(channel in channels)
//				{
//					var animationChannel:MatrixChannel3D = buildAnimationChannel(target, channel);
//					if(animationChannel) 
//						mcs[ channel.syntax.targetSID ] = buildAnimationChannel(target, channel);
//				}
//				
//				var bakedChannel:MatrixChannel3D = new MatrixChannel3D(target);
//				
//				// build a baked channel
//				for(i = 0; i < times.length; i++)
//				{
//					var keyframeTime:Number = times[i];
//					var bakedMatrix:Matrix3D = new Matrix3D();
//					bakedMatrix.identity();
//					
//					// loop over the DaeNode's transform-stack
//					for(var j:int = 0; j < node.transforms.length; j++)
//					{
//						transform = node.transforms[j];
//						
//						var matrixChannel:MatrixChannel3D = mcs[ transform.sid ];
//						
//						if(matrixChannel)
//						{
//							// this transform is animated, so lets determine the matrix for the current keyframeTime
//							var time:Number;
//							if(keyframeTime < matrixChannel.startTime)
//								time = 0;
//							else if(keyframeTime > matrixChannel.endTime)
//								time = 1;
//							else
//								time = keyframeTime / (matrixChannel.endTime - matrixChannel.startTime);
//								
//							// update the channel by time, so the matrix for the current keyframe is setup
//							matrixChannel.updateToTime(time);
//							
//							// bake the matrix
//							bakedMatrix.prepend( target.transform );
//						}
//						else
//						{
//							// this transform isn't animated, simply bake the transform into the matrix
//							bakedMatrix.prepend( buildMatrixFromTransform(transform));
//						}
//					}
//					
//					// now we can add the baked matrix as a new keyframe
//					bakedChannel.addKeyFrame(new AnimationKeyFrame3D("frame_" + i, keyframeTime, [bakedMatrix]));
//				}
//				
//				_channelsByTarget[target] = bakedChannel;
//			}
//		}
//				
//		/**
//		 * 建立单个动画控制器
//		 * 
//		 * Builds a animation channel for an object.
//		 * 
//		 * @param	matrixStackChannel	the target object's channel
//		 * @param	target	The target object
//		 * @param	channel	The DaeChannel
//		 */ 
//		private function buildAnimationChannel(target:Entity, channel:DaeChannel):MatrixChannel3D
//		{
//			//TODO
//			var node:DaeNode = _objectToNode[target];
//					
//			if(!node)
//				throw new Error("Couldn't find the targeted object!");
//					
//			var matrixChannel:MatrixChannel3D = new MatrixChannel3D(target, channel.syntax.targetSID);
//			
//			var transform:DaeTransform = node.findMatrixBySID(channel.syntax.targetSID);
//					
//			if(!transform)
//			{
//				trace("Couldn't find the targeted object's transform: " + channel.syntax.targetSID);
//				return null;
//			}
//			
//			var matrix:Matrix3D = new Matrix3D();
//			var matrixProp:String;
//			var arrayMember:String;
//			var data:Array;
//			var val:Number;
//			var i:int;
//					
//			switch(transform.type)
//			{
//				case "matrix":
//					if(channel.syntax.isFullAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							data = channel.output[i];
//							matrix.rawData = Vector.<Number>(data);
//							matrix.transpose();
//							matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//						}
//					}
//					else if(channel.syntax.isArrayAccess)
//					{
//						arrayMember = channel.syntax.arrayMember.join("");
//						matrix = target.transform.clone();
//						var rawData:Vector.<Number> = new Vector.<Number>(16, true);
//						var n:int;
//						
//						switch(arrayMember)
//						{
//							case "(0)(0)":
//								n = 0; 
//								break;
//							case "(0)(1)":
//								n = 1; 
//								break;
//							case "(0)(2)":
//								n = 2; 
//								break;
//							case "(0)(3)":
//								n = 3; 
//								break;
//							case "(1)(0)":
//								n = 4; 
//								break;
//							case "(1)(1)":
//								n = 5; 
//								break;
//							case "(1)(2)":
//								n = 6; 
//								break;
//							case "(1)(3)":
//								n = 7; 
//								break;
//							case "(2)(0)":
//								n = 8; 
//								break;
//							case "(2)(1)":
//								n = 9; 
//								break;
//							case "(2)(2)":
//								n = 10; 
//								break;
//							case "(2)(3)":
//								n = 11; 
//								break;
//							case "(3)(0)":
//								n = 12; 
//								break;
//							case "(3)(1)":
//								n = 13; 
//								break;
//							case "(3)(2)":
//								n = 14; 
//								break;
//							case "(3)(3)":
//								n = 15; 
//								break;
//							default:
//								throw new Error(arrayMember);
//						}
//						
//						for(i = 0; i < channel.input.length; i++)
//						{
//							rawData[n] = channel.output[i];
//							matrix.rawData = rawData;
//							matrix.transpose();
//							matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [new Matrix3D(rawData)]));
//						}
//					}
//					else
//					{
//						throw new Error("Don't know how to handle this channel: " + channel.syntax);
//					}
//					break;
//				case "rotate":
//					if(channel.syntax.isFullAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							data = channel.output[i];
//							matrix.rawData = Vector.<Number>(data);
//							matrix.transpose();
//							matrix.appendRotation( data[3],new Vector3D( data[0], data[1], data[2] ) );
//							matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//						}
//					}
//					else if(channel.syntax.isDotAccess)
//					{
//						switch(channel.syntax.member)
//						{
//							case "ANGLE":
//								for(i = 0; i < channel.input.length; i++)
//								{
//									matrix.identity();
//									matrix.appendRotation( channel.output[i],new Vector3D( transform.values[0], transform.values[1], transform.values[2] ) );
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//								}
//								break;
//							default:
//								throw new Error("Don't know how to handle this channel: " + channel.syntax);
//						}
//					}
//					else
//					{
//						throw new Error("Don't know how to handle this channel: " + channel.syntax);
//					}	
//					break;
//				case "scale":
//					if(channel.syntax.isFullAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							data = channel.output[i];
//							matrix.identity();
//							matrix.appendScale(data[0], data[1], data[2]);
//							matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//						}
//					}
//					else if(channel.syntax.isDotAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							val = channel.output[i];
//							matrix.identity();
//							switch(channel.syntax.member)
//							{
//								case "X":
//									matrix.appendScale(val, 0, 0);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								case "Y":
//									matrix.appendScale(0, val, 0);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								case "Z":
//									matrix.appendScale(0, 0, val);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								default:
//									break;		
//							}
//						}
//					}
//					else
//					{
//						throw new Error("Don't know how to handle this channel: " + channel.syntax);
//					}
//					break;
//				case "translate":
//					if(channel.syntax.isFullAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							data = channel.output[i];
//							matrix.identity();
//							matrix.appendTranslation(data[0], data[1], data[2]);
//							matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//						}
//					}	
//					else if(channel.syntax.isDotAccess)
//					{
//						for(i = 0; i < channel.input.length; i++)
//						{
//							val = channel.output[i];
//							matrix.identity();
//							switch(channel.syntax.member)
//							{
//								case "X":
//									matrix.appendTranslation(val, 0, 0);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								case "Y":
//									matrix.appendTranslation(0, val, 0);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								case "Z":
//									matrix.appendTranslation(0, 0, val);
//									matrixChannel.addKeyFrame(new AnimationKeyFrame3D("keyframe_" + i, channel.input[i], [matrix.clone()]));
//									break;
//								default:
//									break;		
//							}
//						}
//					}
//					else
//					{
//						throw new Error("Don't know how to handle this channel: " + channel.syntax);
//					}		
//					break;
//				default:
//					throw new Error("Unknown transform type!");	
//			}
//				
//			return matrixChannel;
//		}
//		
//		/**
//		 * 获取动画控制器
//		 * 
//		 * @param	target	The target to get the channels for.
//		 * 
//		 * @return	Array of AnimationChannel3D.
//		 */ 
//		public function getAnimationChannels():Vector.<AbstractChannel3D>
//		{
//			var channels:Vector.<AbstractChannel3D> = new Vector.<AbstractChannel3D>;
//			
//			var channel:AbstractChannel3D
//			for each(channel in _channelsByTarget)
//				channels.push(channel);
//				
//			return channels;
//		}
//		
//		public function getAnimationChannelsByTarget(target:Entity):AbstractChannel3D
//		{
//			return _channelsByTarget[target];
//		}
//		
//		/**
//		 * Called on parse animations progress.
//		 * 
//		 * @param	event
//		 */ 
//		protected function onParseAnimationsProgress(event:ProgressEvent):void
//		{
//			dispatchEvent(event);
//			trace("animations #" + event.bytesLoaded + " of " + event.bytesTotal);
//		}
//		
////		override protected function project(renderData:RenderData):void
////		{
////			var channel:AbstractChannel3D;
////			if(_isPlaying && _channels)
////			{
////				var secs:Number = _currentTime * 0.001;
////				var duration:Number = _endTime - _startTime;
////				var elapsed:Number = (getTimer() * 0.001) - secs;
////				
////				if(elapsed > duration)
////				{
////					_currentTime = getTimer();
////					secs = _currentTime * 0.001;
////					elapsed = 0;
////				}
////				var time:Number = elapsed / duration;
////
////				if (time == 0 && !_loop)
////					stop();
////				else
////				{
////					for each(channel in _channels)
////						channel.updateToTime(time);
////				}
////			}
////
////			super.project(renderData);	
////		}
//		
//		public function stop():void
//		{
//			trace("STOP CALLED ON DAE");
//			_isPlaying = false;	
//			dispatchEvent(new AnimationEvent(AnimationEvent.ANIMATION_COMPLETE, _currentFrame, _totalFrames));
//		}
	}
}
