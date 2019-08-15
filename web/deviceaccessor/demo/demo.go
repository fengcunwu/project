/*
Copyright 2019 The advantech Qing.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package demo


import (
	"reflect"
	"github.com/kubeedge/beehive/pkg/common/log"

	"github.com/kubeedge/kubeedge/cloud/pkg/deviceaccessor/utils"
	"github.com/kubeedge/kubeedge/cloud/pkg/deviceaccessor/manager"
	"github.com/kubeedge/kubeedge/cloud/pkg/apis/devices/v1alpha1"

	"k8s.io/api/core/v1"
	"k8s.io/client-go/rest"
	"k8s.io/apimachinery/pkg/watch"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
)


//Watch/list/Get kubernetes apiserver
type DemoController struct {
	deviceStop chan struct{}
	DeviceAccessor	*manager.DeviceAccessor	
	crdClient 	*rest.RESTClient
}

// CacheDevice is the struct save device data for check device is really changed
type CacheDevice struct {
	metav1.ObjectMeta
	Spec   v1alpha1.DeviceSpec
	Status v1alpha1.DeviceStatus
}

func(dc *DemoController) deviceAdded(device *v1alpha1.Device){
	log.LOGGER.Infof("Device %s is Added!", device.Name)
	// store device 
	dc.DeviceAccessor.Device.Store(device.Name, &CacheDevice{ObjectMeta: device.ObjectMeta, Spec: device.Spec, Status: device.Status})
	
	modbusProtc := device.Spec.Protocol.Modbus
	if modbusProtc.TCP != nil {
	    ip := modbusProtc.TCP.IP
		port := modbusProtc.TCP.Port
		slaveid := modbusProtc.TCP.SlaveID	

		log.LOGGER.Infof("Add device ip=%s, port=%d, slaveid=%s", ip, port, slaveid)
	}

	// through the twins
	for _, twin := range device.Status.Twins {
		name := twin.PropertyName  //as string: property name
		value := twin.Desired.Value  //string  property value
		matadata := twin.Desired.Metadata // map[string]string  meta data, use for range to through the map.
		log.LOGGER.Infof("Property Name:%s, value:%s", name, value)
	
		reportValue := twin.Reported.Value  // string
		reportMetadata := twin.Reported.Metadata // map[string]string	
		log.LOGGER.Infof("Reported Property Name:%s, value:%s, %v, %v", name, reportValue, matadata,reportMetadata)
	}
}

func(dc *DemoController) deviceDeleted(device *v1alpha1.Device){
	log.LOGGER.Infof("Device %s is Deleted!", device.Name)
	//Delete from cache 
	dc.DeviceAccessor.Device.Delete(device.Name)
}

// isDeviceUpdated checks if device is actually updated
func isDeviceUpdated(oldTwin *CacheDevice, newTwin *v1alpha1.Device) bool {
	// does not care fields
	oldTwin.ObjectMeta.ResourceVersion = newTwin.ObjectMeta.ResourceVersion
	oldTwin.ObjectMeta.Generation = newTwin.ObjectMeta.Generation

	// return true if ObjectMeta or Spec or Status changed, else false
	return !reflect.DeepEqual(oldTwin.ObjectMeta, newTwin.ObjectMeta) || !reflect.DeepEqual(oldTwin.Spec, newTwin.Spec) || !reflect.DeepEqual(oldTwin.Status, newTwin.Status)
}

func isDeviceStatusUpdated(oldTwin *CacheDevice, newTwin *v1alpha1.Device) bool {
	
	// return true if Status changed, else false
	return !reflect.DeepEqual(oldTwin.Status, newTwin.Status)
}

func(dc *DemoController) deviceUpdated(device *v1alpha1.Device){
	log.LOGGER.Infof("Device %s is update!", device.Name)

	value, ok := dc.DeviceAccessor.Device.Load(device.Name)
	dc.DeviceAccessor.Device.Store(device.Name, &CacheDevice{ObjectMeta: device.ObjectMeta, Spec: device.Spec, Status: device.Status})

	if ok {
		cacheDevice := value.(*CacheDevice)
		if isDeviceStatusUpdated(cacheDevice, device) {
			name := device.Name
			for _, twin := range device.Status.Twins {
				Value := twin.Desired.Value
				metadata := twin.Desired.Metadata

				log.LOGGER.Infof("Property Name:%s, value:%s, metadata:%v", name, Value, metadata)
			}	
		} 
	}else{
		dc.deviceAdded(device)
	}
}


func(dc *DemoController) deviceLoop(stopch chan struct{}){
	running := true

	for running {
		select {
		case e:= <-dc.DeviceAccessor.Events():
			device, ok := e.Object.(*v1alpha1.Device)
			if !ok {
				log.LOGGER.Infof("Object type: %T is not support", device)
				continue
			}

			switch e.Type {
			case watch.Added:
				dc.deviceAdded(device)
			case watch.Deleted:
				dc.deviceDeleted(device)
			case watch.Modified:
				dc.deviceUpdated(device)
			default:
				log.LOGGER.Warnf("Device event type: %s unsupported", e.Type)

			}
		case <-stopch:
			log.LOGGER.Infof("Stop device loop")
			running =false 		
		}
	}
}

func(dc *DemoController) Start() error {
	log.LOGGER.Infof("Start demo controller")

	dc.deviceStop = make(chan struct{})

	go dc.deviceLoop(dc.deviceStop)

	return nil
}

func NewDemoController() (*DemoController, error){

	config, err := utils.KubeConfig()
	if err != nil {
		log.LOGGER.Warnf("Get kubeConfig error: %v", err)
		return nil, err
	}

	crdcli, err := utils.NewCRDClient(config)
	if err != nil {
		log.LOGGER.Warnf("create crd client error: %v", err)
		return nil, err
	}

	da, err := manager.NewDeviceAccessor(crdcli, v1.NamespaceAll)
	if err != nil {
		log.LOGGER.Warnf("create device accesser error: %v", err)
		return nil, err
	}

	dc := &DemoController{DeviceAccessor:da, crdClient: crdcli}
	
	return dc, err
}
