package cn.alchemy3d.objects.primitives
{
	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Mesh3D;
	import cn.alchemy3d.texture.Texture;
	
	import flash.geom.Point;

	public class Cube extends Mesh3D
	{
		/**
		* 每个轴上的分段数量
		*/
		public var segments:Vector.<Number>;
	
		/**
		* 没有选择任何面
		*/
		static public var NONE:int = -1;
	
		/**
		* 前面
		*/
		static public var FRONT:int = 0;
	
		/**
		* 背面
		*/
		static public var BACK:int = 1;
	
		/**
		* 右面
		*/
		static public var RIGHT:int = 2;
	
		/**
		* 左面
		*/
		static public var LEFT:int = 3;
	
		/**
		* 上面
		*/
		static public var TOP:int = 4;
	
		/**
		* 下面
		*/
		static public var BOTTOM:int = 5;
	
		/**
		* 所有面
		*/
		static public var ALL:int = FRONT + BACK + RIGHT + LEFT + TOP + BOTTOM;
		
		private var insideFaces:int;
		private var excludeFaces:int;
		
		public function Cube(material:Material = null, texture:Texture = null,  width:Number=500, depth:Number=500, height:Number=500, segmentsS:int=1, segmentsT:int=1, segmentsH:int=1, insideFaces:int=0, excludeFaces:int=0, name:String = "")
		{
			super(material, texture, name);
			
			this.insideFaces  = insideFaces;
			this.excludeFaces = excludeFaces;
	
			segments = new Vector.<Number>();
			segments.push(segmentsS);
			segments.push(segmentsT);
			segments.push(segmentsH);
	
			buildCube(width, height, depth);
		}
	
		protected function buildCube( width:Number, height:Number, depth:Number ):void
		{
			var width2  :Number = width * .5;
			var height2 :Number = height * .5;
			var depth2  :Number = depth * .5;
			
			if( ! (excludeFaces & FRONT) )
				buildPlane( material, 0, 1, width, height, depth2, ! Boolean( insideFaces & FRONT ) );
	
			if( ! (excludeFaces & BACK) )
				buildPlane( material, 0, 1, width, height, -depth2, Boolean( insideFaces & BACK ) );
	
			if( ! (excludeFaces & RIGHT) )
				buildPlane( material, 2, 1, depth, height, width2, Boolean( insideFaces & RIGHT ) );
	
			if( ! (excludeFaces & LEFT) )
				buildPlane( material, 2, 1, depth, height, -width2, ! Boolean( insideFaces & LEFT ) );
	
			if( ! (excludeFaces & TOP) )
				buildPlane( material, 0, 2, width, depth, height2, Boolean( insideFaces & TOP ) );
	
			if( ! (excludeFaces & BOTTOM) )
				buildPlane( material, 0, 2, width, depth, -height2, ! Boolean( insideFaces & BOTTOM ) );
			
			//合并共有顶点
//			mergeVertices();
			
			applyForMeshBuffer();
			
			initialize();
		}
	
		protected function buildPlane(mat:Material, u:int, v:int, width:Number, height:Number, depth:Number, reverse:Boolean=false):void
		{
			// Find w depth axis
			var w :int;
			if( (u==0 && v==1) || (u==1 && v==0) ) w = 2;
			else if( (u==0 && v==2) || (u==2 && v==0) ) w = 1;
			else if( (u==2 && v==1) || (u==1 && v==2) ) w = 0;
	
			// Mirror
			var rev :Number = reverse? -1 : 1;
	
			// Build plane
			var gridU    :Number = this.segments[ u ];
			var gridV    :Number = this.segments[ v ];
			var gridU1   :Number = gridU + 1;
			var gridV1   :Number = gridV + 1;
	
			var vertices   :Vector.<Vertex3D> = this.vertices;
			var faces      :Vector.<Triangle3D> = this.faces;
			var planeVerts :Vector.<Vertex3D> = new Vector.<Vertex3D>;
	
			var textureU :Number = width * .5;
			var textureV :Number = height * .5;
	
			var incU     :Number = width / gridU;
			var incV     :Number = height / gridV;
	
			// Vertices
			for( var iu:int = 0; iu < gridU1; iu++ )
			{
				for( var iv:int = 0; iv < gridV1; iv++ )
				{
					var vertex:Vertex3D = new Vertex3D(0, 0, 0, 1);
					
					var vx:Number = (iu * incU - textureU) * rev;
					var vy:Number = iv * incV - textureV;
					var vz:Number = depth;
					
					if (u == 0) vertex.x = vx;
					else if (u == 1) vertex.y = vx;
					else if (u == 2) vertex.z = vx;
					
					if (v == 0) vertex.x = vy;
					else if (v == 1) vertex.y = vy;
					else if (v == 2) vertex.z = vy;
					
					if (w == 0) vertex.x = vz;
					else if (w == 1) vertex.y = vz;
					else if (w == 2) vertex.z = vz;
					
					vertices.push(vertex);
					planeVerts.push(vertex);
				}
			}
	
			// Faces
			var uvA :Point;
			var uvC :Point;
			var uvB :Point;
			
			var aIndex:int;
			var cIndex:int;
			var bIndex:int;
			
			var count:int = 0;
			
			for(  iu = 0; iu < gridU; iu++ )
			{
				for(  iv= 0; iv < gridV; iv++ )
				{
					// Triangle A
					var a:Vertex3D = planeVerts[ iu     * gridV1 + iv     ];
					var c:Vertex3D = planeVerts[ iu     * gridV1 + (iv+1) ];
					var b:Vertex3D = planeVerts[ (iu+1) * gridV1 + iv     ];
	
					uvA =  new Point( iu     / gridU, iv     / gridV );
					uvC =  new Point( iu     / gridU, (iv+1) / gridV );
					uvB =  new Point( (iu+1) / gridU, iv     / gridV );
	
					faces.push(new Triangle3D(a, b, c, uvA, uvB, uvC, this, "Triangle" + count));
	
					// Triangle B
					a = planeVerts[ (iu+1) * gridV1 + (iv+1) ];
					c = planeVerts[ (iu+1) * gridV1 + iv     ];
					b = planeVerts[ iu     * gridV1 + (iv+1) ];
	
					uvA =  new Point( (iu+1) / gridU, (iv+1) / gridV );
					uvC =  new Point( (iu+1) / gridU, iv     / gridV );
					uvB =  new Point( iu     / gridU, (iv+1) / gridV );
	
					faces.push(new Triangle3D(c, a, b, uvC, uvA, uvB, this, "Triangle" + (count + 1)));
					
					count += 2;
				}
			}
		}
	}
}