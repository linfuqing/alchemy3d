package cn.alchemy3d.tools
{
	import flash.utils.describeType;

	public class Ransack
	{
		public function Ransack()
		{
		}

		public static function traceWith(target:*, textTarget:*):void
		{
			var instanceInfo:XML=describeType(target);
			var properties:XMLList=instanceInfo.accessor.(@access != "writeonly") + instanceInfo.variable;
			var startN:Number=0;
			var Swidth:Number=20;
			var Smax:Number=0;
			var Tmax:Number=15;
			//trace(getDefinitionByName(target) +">"+getQualifiedClassName(target));
			for each (var propertyInfo:XML in properties)
			{
				Smax++;
				var propertyName:String=propertyInfo.@name;
				var Slength:Number=propertyName.toString().length;
				if (Slength > (Swidth - 5))
				{
					Swidth=Slength + 5;
				}
				var nowType:String=typeof(target[propertyName]);
				var Tlength:Number=nowType.length;
				if (Tlength > (Tmax - 5))
				{
					Tmax=Tlength + 5;
				}
			}
			var titleString:String="-----------";
			if (Smax > 10)
			{
				var NM:Number=Smax.toString().length;
				for (var m:Number=0; i < (NM - 1); m++)
				{
					;
					titleString+="-";
				}
				titleString+="[ 属性名称 ]";
				for (var sw:Number=0; sw < (Swidth - 12); sw++)
				{
					titleString+="-";
				}
				titleString+="[ 属性类型 ]---";
				if (Tmax > 15)
				{
					for (var tw:Number=0; tw < (Tmax - 15); tw++)
					{
						titleString+="-";
					}
				}
				titleString+="[ 属性值 ]----------------------------------------";
				trace(titleString);
				if (textTarget)
				{
					textTarget.htmlText+="<br/>" + titleString
				}
				//trace("-----------[ 属性名称 ]------------------[ 属性值 ]------------------------------");
				for each (propertyInfo in properties)
				{
					startN++;
					// 取出属性名
					propertyName=propertyInfo.@name;
					nowType=typeof(target[propertyName]);
					// 根据属性名来访问
					var traceString:String="[ ";
					if (Smax > 10)
					{
						var NN:Number=NM - startN.toString().length;
						for (var n:Number=0; n < NN; n++)
						{
							;
							traceString+=" ";
						}
						traceString+=startN + " ]      ";
						traceString+=propertyName;
						Slength=propertyName.toString().length;
						if (Slength < Swidth)
						{
							for (var i:Number=0; i < (Swidth - Slength); i++)
							{
								;
								traceString+=" ";
							}
							traceString+=nowType;
							//if (Tmax>15) {
							var Tnow:Number=nowType.length;
							//}
							for (var tn:Number=0; tn < (Tmax - Tnow); tn++)
							{
								;
								//dfd
								traceString+=" ";
							}
							traceString+=target[propertyName];
							trace(traceString);
							if (textTarget)
							{
								textTarget.htmlText+="<br/>" + traceString
							}

						}
					}
				}
			}
		}
	}
}
