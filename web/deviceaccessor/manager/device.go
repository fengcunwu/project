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

package manager


import (
	"sync"
	"encoding/json"

	"k8s.io/client-go/rest"
	"k8s.io/client-go/tools/cache"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/apimachinery/pkg/watch"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"

	"github.com/kubeedge/kubeedge/cloud/pkg/apis/devices/v1alpha1"
)
const (
	DefaultDeviceEventBuffer      = 1
	// ResourceTypeDevices is plural of device resource in apiserver
	ResourceTypeDevices = "devices"
	
	MergePatchType = "application/merge-patch+json"
)

// DeviceStatus is structure to patch device status
type DeviceStatus struct {
	Status v1alpha1.DeviceStatus `json:"status"`
}

type DeviceUpdate struct {
	DeviceID  string
	Status *DeviceStatus
}
// DeviceAccessor is Accessor to access device resource in remote k8s apiserver
type DeviceAccessor struct{

	//client is the kubernets apiserver's client
	client *rest.RESTClient
	//events from watch kubernets apiserver.
	events chan watch.Event

	//Device is cache, key is device.Name, value is *v1alpha1.Device{} 
	Device sync.Map

	//DevLister is interface to list/Get device object.
	DevLister DeviceLister
}

// Events return a channel, can receive all device event
func (da *DeviceAccessor) Events() chan watch.Event {
	return da.events
}

//CreateDevice create the k8s device resource.
func (da *DeviceAccessor) CreateDevice(device *v1alpha1.Device, namespace string) error{

	return da.client.Post().
		Namespace(namespace).
		Resource("devices").
		Body(device).
		Do().
		Error()
}

//UpdateDeviceStatus update the k8s devicestatus resource.
func (da *DeviceAccessor) UpdateDeviceStatus(device *DeviceUpdate, namespace string) error{

	body, err := json.Marshal(device.Status)
	if err != nil {
		return err
	}

	return da.client.Patch(MergePatchType).
		Namespace(namespace).
		Resource("devices").
		Name(device.DeviceID).
		Body(body).
		Do().
		Error()
}

//DeleteDevice delete a devcie
func (da *DeviceAccessor) DeleteDevice(name, namespace string, options *metav1.DeleteOptions) error {

	return da.client.Delete().
		Namespace(namespace).
		Resource("devices").
		Name(name).
		Body(options).
		Do().
		Error()
}

func NewDeviceAccessor(crdClient *rest.RESTClient, namespace string) (*DeviceAccessor, error) {

	//1. Create listWatcher for watch the device object's add/delete/update
	lw := cache.NewListWatchFromClient(crdClient, "devices", namespace, fields.Everything())

	//2. Create the event channel for recieve watch.Evant Object.
	events := make(chan watch.Event, DefaultDeviceEventBuffer)

	//3. wrapper the events into resourceeventhandler with OnAdd/OnDelete/OnUpdate
	rh := NewCommonResourceEventHandler(events)

	//4. create shared informer.
	si := cache.NewSharedInformer(lw, &v1alpha1.Device{}, 0)

	//5. add the eventhandler to shared informer.
	si.AddEventHandler(rh)	

	sif := si.(cache.SharedIndexInformer)
	da := &DeviceAccessor{client: crdClient, DevLister: NewDeviceLister(namespace, sif.GetIndexer()), events: events}


	stopNever := make(chan struct{})
	go si.Run(stopNever)


	return da, nil
}
