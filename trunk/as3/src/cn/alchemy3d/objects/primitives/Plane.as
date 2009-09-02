package cn.alchemy3d.objects.primitives
{

	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.materials.Material;
	import cn.alchemy3d.objects.Mesh3D;
	import cn.alchemy3d.texture.Texture;
	
	import flash.geom.Point;
	
	public class Plane extends Mesh3D
	{
		/**
		 * 构造函数
		 * 
		 * @param material 材质
		 * @param width 宽度
		 * @param _height 高度
		 * @param segmentsWidth 水平方向分段
		 * @param segments_height 垂直方向分段
		 * @param name 名字
		 */
		public function Plane(material:Material = null, texture:Texture = null, width:Number = 500, height:Number = 500, segments_width:uint = 1, segments_height:uint = 1, name:String = "")
		{
			this._width = width;
			this._height = height;
			this._segments_width = segments_width;
			this._segments_height = segments_height;
			
			super(material, texture, name);
		}
		
		override protected function initialize():void
		{
			buildPlane();
			
			applyForMeshBuffer();
			
			super.initialize();
			
//			flipFaces();
		}
		
		private var _height:Number;
		private var _segments_height:uint;
		private var _segments_width:uint;
		private var _width:Number;
		
		protected function buildPlane():void
		{
			var gridX:Number = this._segments_width;
			var gridY:Number = this._segments_height;
			var gridX1:Number = gridX + 1;
			var gridY1:Number = gridY + 1;
	
			var textureX :Number = _width * .5;
			var textureY :Number = _height * .5;
	
			var iW:Number = _width / gridX;
			var iH:Number = _height / gridY;
			
			// Vertices
			for( var ix:int = 0; ix < gridX + 1; ix++ )
			{
				for( var iy:int = 0; iy < gridY1; iy++ )
				{
					var x :Number = ix * iW - textureX;
					var y :Number = iy * iH - textureY;
	
					vertices.push(new Vertex3D(x, y, 0, 1));
				}
			}
			
			// Faces
			var uvA :Point;
			var uvC :Point;
			var uvB :Point;
			
			var a:Vertex3D;
			var c:Vertex3D;
			var b:Vertex3D;
			
			var aIndex:int;
			var cIndex:int;
			var bIndex:int;
			
			var count:int = 0;
			for(  ix = 0; ix < gridX; ix++ )
			{
				for(  iy= 0; iy < gridY; iy++ )
				{
					// Triangle A
					aIndex = ix     * gridY1 + iy;
					cIndex = ix     * gridY1 + (iy+1);
					bIndex = (ix+1) * gridY1 + iy;
					
					a = vertices[aIndex];
					c = vertices[cIndex];
					b = vertices[bIndex];
	
					uvA =  new Point( ix     / gridX, iy     / gridY );
					uvC =  new Point( ix     / gridX, (iy+1) / gridY );
					uvB =  new Point( (ix+1) / gridX, iy     / gridY );
	
					faces.push(new Triangle3D(a, b, c, uvA, uvB, uvC, this, "Triangle" + count));
	
					// Triangle B
					aIndex = (ix+1) * gridY1 + (iy+1);
					cIndex = (ix+1) * gridY1 + iy;
					bIndex = ix     * gridY1 + (iy+1);
					
					a = vertices[aIndex];
					c = vertices[cIndex];
					b = vertices[bIndex];
	
					uvA =  new Point( (ix+1) / gridX, (iy+1) / gridY );
					uvC =  new Point( (ix+1) / gridX, iy      / gridY );
					uvB =  new Point( ix      / gridX, (iy+1) / gridY );
					
					faces.push(new Triangle3D(a, b, c, uvA, uvB, uvC, this, "Triangle" + (count + 1)));
					
					count += 2;
				}
			}
		}

	}
}