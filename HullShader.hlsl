[domain('tri')]
[partitioning('integer')]
[outputtopology('triangle_cw')]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]



HullOutputType ColorHullShader(inputPatch<HullinputType, 3> patch, uint pointId : SV_outputControlPointID, uint patchId : SV_primitiveID)
{
	HullOutputType output;

	


}