package cn.alchemy3d.polygon.primitives
{

	import flash.geom.Point;
	import flash.geom.Vector3D;
	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.polygon.Mesh;
	
	/**
	 * Plane是一个3D平面，拥有位移、旋转、缩放操作
	 */
	public class Plane extends Mesh
	{
		/**
		 * 构造函数
		 * 
		 * @param material 材质
		 * @param width 宽度
		 * @param height 高度
		 * @param segmentsWidth 水平方向分段
		 * @param segmentsHeight 垂直方向分段
		 * @param name 名字
		 */
		public function Plane(materialID:int = -1, width:Number = 500, height:Number = 500, segmentsWidth:uint = 1, segmentsHeight:uint = 1, name:String = "")
		{
			super(name);
			
			this.materialID = materialID;
			
			this.width = width;
			this.height = height;
			this.segmentsWidth = segmentsWidth;
			this.segmentsHeight = segmentsHeight;
			
			buildPlane();
			
			geometry.computeTriNormals();
			geometry.computeVertexNormals();
		}
		
		private var height:Number;
		private var segmentsHeight:uint;
		private var segmentsWidth:uint;
		private var width:Number;
		
		protected function buildPlane():void
		{
			var gridX:Number = this.segmentsWidth;
			var gridY:Number = this.segmentsHeight;
			var gridX1:Number = gridX + 1;
			var gridY1:Number = gridY + 1;
	
			var textureX :Number = width * .5;
			var textureY :Number = height * .5;
	
			var iW:Number = width / gridX;
			var iH:Number = height / gridY;
			
			// Vertices
			for( var ix:int = 0; ix < gridX + 1; ix++ )
			{
				for( var iy:int = 0; iy < gridY1; iy++ )
				{
					var x :Number = ix * iW - textureX;
					var y :Number = iy * iH - textureY;
	
					vertices.push(new Vertex3D(x, y, 0, this));
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
	
					faces.push(new Triangle3D(materialID, aIndex, bIndex, cIndex, uvA, uvB, uvC, this, "Triangle" + count));
	
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
					
					faces.push(new Triangle3D(materialID, aIndex, bIndex, cIndex, uvA, uvB, uvC, this, "Triangle" + (count + 1)));
					
					count += 2;
				}
			}
		}

	}
}