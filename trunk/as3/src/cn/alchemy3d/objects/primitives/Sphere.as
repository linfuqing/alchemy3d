package cn.alchemy3d.objects.primitives
{

	import flash.geom.Point;
	import flash.utils.Dictionary;
	import cn.alchemy3d.geom.Triangle3D;
	import cn.alchemy3d.geom.Vertex3D;
	import cn.alchemy3d.polygon.Mesh3D;
	

	public class Sphere extends Mesh3D
	{
		public static  var DEFAULT_RADIUS :Number = 100;
		public static  var DEFAULT_SEGMENTSH :Number = 6;
		public static  var DEFAULT_SEGMENTSW :Number = 8;
		public static  var MIN_SEGMENTSH :Number = 2;
		public static  var MIN_SEGMENTSW :Number = 3;

		public function Sphere(materialID:int = -1, radius:Number = 100, segmentsW:int = 8, segmentsH:int = 6, name:String = "")
		{
			super(name);
			
			this.materialID = materialID;
	
			this.segmentsW = Math.max( MIN_SEGMENTSW, segmentsW || DEFAULT_SEGMENTSW); // Defaults to 8
			this.segmentsH = Math.max( MIN_SEGMENTSH, segmentsH || DEFAULT_SEGMENTSH); // Defaults to 6
			
			if (radius==0) radius = DEFAULT_RADIUS;
	
			buildSphere( radius );
			
			applyForTmpBuffer();
		}
		
		private var segmentsH :Number;
		private var segmentsW :Number;
		
		private var v:Dictionary = new Dictionary();
	
		private function buildSphere( fRadius:Number ):void
		{
			var i:Number, j:Number, k:Number;
			
			var iHor:Number = Math.max(3,this.segmentsW);
			var iVer:Number = Math.max(2,this.segmentsH);
			
			var aVtc:Array = new Array();
			var vv:int = 0;
			
			for (j = 0; j < (iVer+1); j ++)
			{
				// vertical
				var fRad1:Number = Number(j/iVer);
				var fZ:Number = -fRadius*Math.cos(fRad1*Math.PI);
				var fRds:Number = fRadius*Math.sin(fRad1*Math.PI);
				var aRow:Array = new Array();
				var oVtx:Vertex3D;
				
				for (i = 0; i < iHor; i ++)
				{
					// horizontal
					var fRad2:Number = Number(2*i/iHor);
					var fX:Number = fRds*Math.sin(fRad2*Math.PI);
					var fY:Number = fRds*Math.cos(fRad2*Math.PI);
					if (!((j==0||j==iVer)&&i>0))
					{
						// top||bottom = 1 vertex
						oVtx = new Vertex3D(fY, fZ, fX, this);
						vertices.push(oVtx);
						v[oVtx] = vv;
						vv ++;
					}
					aRow.push(oVtx);
				}
				aVtc.push(aRow);
			}
			
			var iVerNum:int = aVtc.length;
			for (j = 0; j < iVerNum; j ++)
			{
				var iHorNum:int = aVtc[j].length;
				if (j > 0)
				{
					// &&i>=0
					for (i = 0; i < iHorNum; i ++)
					{
						// select vertices
						var bEnd:Boolean = i==(iHorNum-0);
						
						var aP1:Vertex3D = aVtc[j][bEnd?0:i];
						var aP2:Vertex3D = aVtc[j][(i==0?iHorNum:i)-1];
						var aP3:Vertex3D = aVtc[j-1][(i==0?iHorNum:i)-1];
						var aP4:Vertex3D = aVtc[j-1][bEnd?0:i];
						
						var aP1Index:int = v[aP1];
						var aP2Index:int = v[aP2];
						var aP3Index:int = v[aP3];
						var aP4Index:int = v[aP4];
						
						// uv
						/*
						 * fix applied as suggested by Philippe to correct the uv mapping on a sphere
						 * */
						var fJ0:Number = j		/ (iVerNum-1);
						var fJ1:Number = (j-1)	/ (iVerNum-1);
						var fI0:Number = (i+1)	/ iHorNum;
						var fI1:Number = i		/ iHorNum;
						
						var aP4uv:Point = new Point(fI0,fJ1);
						var aP1uv:Point = new Point(fI0,fJ0);
						var aP2uv:Point = new Point(fI1,fJ0);
						var aP3uv:Point = new Point(fI1,fJ1);
						
						// 2 faces
						if (j<(aVtc.length-1))	faces.push(new Triangle3D(materialID, aP1Index, aP2Index, aP3Index, aP1uv, aP2uv, aP3uv, this));
						if (j>1)				faces.push(new Triangle3D(materialID, aP1Index, aP3Index, aP4Index, aP1uv, aP3uv, aP4uv, this));
	
					}
				}
			}
		}

	}
}