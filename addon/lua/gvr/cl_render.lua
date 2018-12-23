CreateConVar("vr_draw_monitor", "0", FCVAR_ARCHIVE)

local w,h = ScrW(),ScrH()

local eyeDist = 2
local part = 2

local height = nil

local HMDMat = Material("vr/hmd.png")

local function DrawVR(eyepos, eyeang)
	if (!GVR.isReady) then return end

	local oldRT = render.GetRenderTarget()

	OpenVR.UpdateTracking()

	local vectors = OpenVR.GetHeadsetVector() or Vector(0,0,0)
	local angles = OpenVR.GetHeadsetAngle() or Angle(0,0,0)
	if (!height) then
		height = vectors.y
	end
	local ang = Angle(angles.pitch * -120, angles.yaw * 120 + eyeang.yaw, angles.roll * -120)
	local vec = Vector(vectors.x * -50, vectors.z * 50, (vectors.y - height) * 50)

	vec:Rotate(Angle(0,ang.yaw,0))

	eyepos = eyepos + Vector(0,20,20)
	eyeang = ang
	//view.fov = fov


	local dist = eyeang:Right() * eyeDist
	dist:Rotate(Angle(0,0,ang.roll))

	local lEye = GetRenderTargetEx("VR_LEFT", w, h, RT_SIZE_FULL_FRAME_BUFFER, MATERIAL_RT_DEPTH_SEPARATE, 0, CREATERENDERTARGETFLAGS_HDR, IMAGE_FORMAT_DEFAULT)
	render.SetRenderTarget(lEye)

	//Left
	render.RenderView({
		origin = eyepos + dist,
		angles = eyeang,
		x = 0,
		y = 0,
		w = w,
		h = h
	})

	local rEye = GetRenderTargetEx("VR_RIGHT", w, h, RT_SIZE_FULL_FRAME_BUFFER, MATERIAL_RT_DEPTH_SEPARATE, 0, CREATERENDERTARGETFLAGS_HDR, IMAGE_FORMAT_DEFAULT)
	render.SetRenderTarget(rEye)

	//Right
	render.RenderView({
		origin = eyepos - dist,
		angles = eyeang,
		x = 0,
		y = 0,
		w = w,
		h = h
	})

	OpenVR.Render(lEye, rEye)

	render.SetRenderTarget(oldRT)

	if (GetConVar("vr_draw_monitor"):GetBool()) then
		return
	else
		render.SetMaterial(HMDMat)
		render.DrawScreenQuad()

		return true
	end
end
hook.Add("RenderScene", "RenderVR", DrawVR)